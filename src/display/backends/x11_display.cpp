
#include <cstring>
#include <optional>
#include <sys/select.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>

#include "display/common/display_controler.h"
#include "display/common/widget_adapter.h"
#include "x11_display.h"

#include <iostream>

namespace View {

    class x11_window : private widget_adapter {

        static constexpr auto X_EVENT_MASK =
            KeyPressMask        |
            KeyReleaseMask      |
            ExposureMask        |
            StructureNotifyMask |
            LeaveWindowMask     |
            PointerMotionMask   |
            ButtonPressMask     |
            ButtonReleaseMask   |
            EnterWindowMask     |
            ConfigureNotify;

    public:
        x11_window(Window parent, widget& root, float pixel_per_unit);
        x11_window(x11_window&) = delete;
        ~x11_window();

        /**
         *  \brief wait and process some events
         *  \param usec_wait_timeout the maximum event wait time allowed
         *  \return true if windows must be closed else false
         **/
        bool process_events(unsigned int usec_wait_timeout);

    private:
        //  Widget adapter interface
        void sys_invalidate_rect(const draw_area& area) override;

        //  Internal helpers
        void _resize_window(unsigned int width, unsigned int height);
        bool _process_event(const XEvent& event, std::optional<draw_area>& redraw_area);
        void _redraw_area(draw_area area);
        void _redraw_window();

        Display *display{nullptr};
        Window window{0};
        Visual *visual{0};
        XdbeBackBuffer back_buffer{0};            // For double buffering
        Atom wm_delete_message{0};
        cairo_surface_t *cairo_surface{nullptr};
		cairo_t *cr{nullptr};
    };

    x11_window::x11_window(Window parent, widget& root, float pixel_per_unit)
    :   widget_adapter{root, pixel_per_unit}
    {
        const auto width = display_width();
        const auto height = display_height();

        /** \todo check error **/
        display = XOpenDisplay(nullptr /* display name*/);
        Window root_window = DefaultRootWindow(display);

        //  Check if XBde extension is supported
        int major, minor;

        if (XdbeQueryExtension(display, &major, &minor) == 0)
            throw ::std::runtime_error("View::x11_window : Xbde extension is not supported");

        //  Check if root window support double buffering
        int drawable_count = 1;
        XdbeScreenVisualInfo *visual_info =
            XdbeGetVisualInfo(display, &root_window /* array of one element*/, &drawable_count);

        if (visual_info == nullptr || visual_info->count == 0 || drawable_count != 1)
            throw ::std::runtime_error("View::x11_window : root windows visual do not support double buffering");

        //  Look for a screen that support double buffering
        XVisualInfo screen_info_template;

        screen_info_template.visualid = visual_info->visinfo[0].visual;
        screen_info_template.screen = 0;
        screen_info_template.depth = visual_info->visinfo[0].depth;

        int found_screen_count = 0;
        XVisualInfo *found_screen_info =
            XGetVisualInfo(display,
                VisualIDMask | VisualScreenMask | VisualDepthMask,
                &screen_info_template, &found_screen_count);

        if( found_screen_info == nullptr || found_screen_count < 1 )
            throw std::runtime_error("View::x11_window : No Visual With Double Buffering\n");

        visual = found_screen_info->visual;

        //  Create the xindows
        const auto screen_id = DefaultScreen(display);
        XSetWindowAttributes xattributs;
        std::memset(&xattributs, 0, sizeof(xattributs));

        xattributs.background_pixel = WhitePixel(display, screen_id);

        window =
            XCreateWindow(
                display, root_window,
                0, 0, width, height, 0,
                CopyFromParent, CopyFromParent,
                found_screen_info->visual,
                CWBackPixel, &xattributs);

        //  Reparrent the windows if a parent was given
        if (parent != 0)
            XReparentWindow(display, window, parent, 0, 0);

        //  Handle windows manager close event
        wm_delete_message = XInternAtom(display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(display, window, &wm_delete_message, 1);

        /** \todo Check errors **/
        //  Allocate back buffer for double buffering
        back_buffer = XdbeAllocateBackBufferName(display, window, XdbeBackground);

        //  Select which type of event should be processed
        XSelectInput(display, window, X_EVENT_MASK);

        //  Map (show) the windows and wait untile it is mapped
        XMapWindow(display, window);
        XEvent event;
        do {
            XNextEvent(display, &event);
        } while (event.type != MapNotify);

        //  Create cairo surface
        cairo_surface =
            cairo_xlib_surface_create(
                display, back_buffer, found_screen_info->visual, width, height);

        cr = cairo_create(cairo_surface);

        free(visual_info);
        free(found_screen_info);

        //  Initial windows drawing
        _redraw_window();
    }

    x11_window::~x11_window()
    {
        cairo_destroy(cr);
        cairo_surface_destroy(cairo_surface);
        XdbeDeallocateBackBufferName(display, back_buffer);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }

    bool x11_window::process_events(unsigned int usec_wait_timeout)
    {
        std::optional<draw_area> redraw_area;

        struct timeval timeout = {
                .tv_sec = 0,
                .tv_usec = usec_wait_timeout };

        int event_queue_fd = ConnectionNumber(display);

        fd_set set;
        FD_ZERO(&set);
        FD_SET(event_queue_fd, &set);

        int rc;

        while ((rc = select(event_queue_fd + 1, &set, 0, 0, &timeout)) != 0) {
            if (rc < 0) {
                //  Select failed, close windows
                return true;
            }
            else {
                //  There are some event to be processed
                bool need_redraw = false;

                while (XPending(display)) {
                    XEvent event;
                    XNextEvent(display, &event);
                    if (_process_event(event, redraw_area))
                        return true;
                }

                if (redraw_area)
                    _redraw_area(redraw_area.value());
            }
        }

        return false;
    }

    void x11_window::_resize_window(unsigned int width, unsigned int height)
    {
        std::cout << "Resize window : from ["
            << display_width() << "x" << display_height() << "]"
            << " to [" << width << "x"  << height << "]" << std::endl;

        resize_display(width, height);

        cairo_destroy(cr);
        cairo_surface_destroy(cairo_surface);
        XdbeDeallocateBackBufferName(display, back_buffer);

        XResizeWindow(display, window, width, height);
        back_buffer = XdbeAllocateBackBufferName(display, window, XdbeBackground);
        cairo_surface =
            cairo_xlib_surface_create(
                display, back_buffer, visual, width, height);

        cr = cairo_create(cairo_surface);

        _redraw_window();
    }

    bool x11_window::_process_event(const XEvent& event, std::optional<draw_area>& redraw_area)
    {

        switch (event.type)
        {

        case ButtonPress:
            switch (event.xbutton.button)
            {
                case 1: sys_mouse_button_down(mouse_button::left);  break;
                case 2: sys_mouse_button_down(mouse_button::wheel); break;
                case 3: sys_mouse_button_down(mouse_button::right); break;
                case 4: sys_mouse_wheel(1.0f);  break;
                case 5: sys_mouse_wheel(-1.f);  break;
            }
        break;

        case ButtonRelease:
        {
            /** \todo DoubleClick !! **/
            switch (event.xbutton.button)
            {
                case 1: sys_mouse_button_up(mouse_button::left);  break;
                case 2: sys_mouse_button_up(mouse_button::wheel); break;
                case 3: sys_mouse_button_up(mouse_button::right); break;
            }
        }
        break;

        case KeyPress:
            /** \todo **/
        break;

        case KeyRelease:
            /** \todo **/
        break;

        case MotionNotify:
            sys_mouse_move(
                static_cast<unsigned int>(event.xmotion.x),
                static_cast<unsigned int>(event.xmotion.y));
        break;

        case Expose:
            {
                const auto exposed_area = make_rectangle(
                    event.xexpose.y, event.xexpose.y + event.xexpose.height,
                    event.xexpose.x, event.xexpose.x + event.xexpose.width);

                //  Update redraw area
                if (redraw_area)
                    redraw_area = exposed_area.bounding(redraw_area.value());
                else
                    redraw_area = exposed_area;
            }
        break;

        case EnterNotify:
            sys_mouse_enter();
        break;

        case LeaveNotify:
            sys_mouse_exit();
        break;

        case ClientMessage:
            if (event.xclient.data.l[0] == wm_delete_message)
                return true;    //  Windo<s must be closed
        break;

        case ConfigureNotify:
        {
            const auto& conf = event.xconfigure;

            //  Windows resized by user
            if (conf.width != display_width() || conf.height != display_height())
                _resize_window(conf.width, conf.height);
        }
        break;

        }

        return false;
    }

    void x11_window::_redraw_area(draw_area area)
    {
        const auto window_area = make_rectangle(0, display_height(), 0, display_width());

        //  Compute intesection beetween area and windows (what we actually need to redraw)
        draw_area drawing_area;
        if (area.intersect(window_area, drawing_area)) {
            std::cout << "Redraw area " << drawing_area << std::endl;

            //  Redraw
            sys_draw_rect(cr, drawing_area.top, drawing_area.bottom, drawing_area.left, drawing_area.right);

            // Double buffering : Swap Buffers
            XdbeSwapInfo info;
            info.swap_window = window;
            info.swap_action = XdbeBackground;
            XdbeSwapBuffers(display, &info, 1);
            XFlush(display);
        }
    }

    void x11_window::_redraw_window()
    {
        sys_draw(cr);
        XdbeSwapInfo info;
        info.swap_window = window;
        info.swap_action = XdbeBackground;
        XdbeSwapBuffers(display, &info, 1);
        XFlush(display);
    }

    void x11_window::sys_invalidate_rect(const draw_area& area)
    {
        XEvent ev;
        std::memset(&ev, 0, sizeof(ev));

        ev.type = Expose;

        ev.xexpose.x = area.left;
        ev.xexpose.y = area.top;
        ev.xexpose.width = area.right - area.left;
        ev.xexpose.height = area.bottom - area.top;
        ev.xexpose.window = back_buffer;

        XSendEvent(display, window, true, ExposureMask, &ev);
    }

    /**
     *
     *      x11_display implementation
     *
     */

    x11_display::x11_display(widget& root, float pixel_per_unit)
    : _root{root}, _pixel_per_unit{pixel_per_unit}
    {

    }

    void x11_display::_create_window(Window parent)
    {
        _running = true;
        _window_thread =
            std::thread{_window_proc, this, parent};
    }

    void x11_display::_wait_window_thread()
    {
        if (_window_thread.joinable())
            _window_thread.join();
    }

    void x11_display::_close_window()
    {
        _running = false;
    }

    void x11_display::_window_proc(x11_display *self, Window parent)
    {
        constexpr auto event_timeout = 100000;
        x11_window win{parent, self->_root, self->_pixel_per_unit};

        while (self->_running) {
            if (win.process_events(event_timeout))
                self->_running = false;
        }
    }
}
