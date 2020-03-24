
#include <cstring>
#include <optional>
#include <array>

#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

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

        //  display controler interface
        void set_cursor(cursor c) override;

    private:
        //  Widget adapter interface
        void sys_invalidate_rect(const draw_area& area) override;

        //  Internal helpers
        void _allocate_drawing_context(unsigned int width, unsigned int height);
        void _free_drawing_context();

        void _resize_window(unsigned int width, unsigned int height);
        bool _process_event(const XEvent& event, std::optional<draw_area>& redraw_area);

        void _redraw_area(draw_area area);
        void _redraw_window();
        void _apply_draw_buffer();

        void _initialiaze_cursors();
        void _free_cursors();

        //  X11 members
        Display *display{nullptr};
        Window window{0};
        Visual *visual{0};
        Atom wm_delete_message{0};
        std::array<Cursor, VIEW_CURSOR_COUNT> x11_cursors{};

        //  dbl click detection
        Time _last_click_time{};

        //  Cairo members
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

        visual = XDefaultVisual(display, DefaultScreen(display));

        //  Create the xindows
        const auto screen_id = DefaultScreen(display);
        XSetWindowAttributes xattributs;
        std::memset(&xattributs, 0, sizeof(xattributs));

        xattributs.background_pixel = BlackPixel(display, screen_id);

        window =
            XCreateWindow(
                display, root_window,
                0, 0, width, height, 0,
                CopyFromParent, CopyFromParent,
                visual,
                CWBackPixel, &xattributs);

        //  Reparrent the windows if a parent was given
        if (parent != 0)
            XReparentWindow(display, window, parent, 0, 0);

        //  Handle windows manager close event
        wm_delete_message = XInternAtom(display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(display, window, &wm_delete_message, 1);

        //  Select which type of event should be processed
        XSelectInput(display, window, X_EVENT_MASK);

        //  Initialazie cursors
        _initialiaze_cursors();

        //  Map (show) the windows and wait untile it is mapped
        XMapWindow(display, window);
        XEvent event;
        do {
            XNextEvent(display, &event);
        } while (event.type != MapNotify);

        //  Prepare drawing context
        cairo_surface =
            cairo_xlib_surface_create(
                display, window, visual, width, height);

        cr = cairo_create(cairo_surface);

        //  Initial windows drawing
        _redraw_window();
    }

    x11_window::~x11_window()
    {
        cairo_destroy(cr);
        cairo_surface_destroy(cairo_surface);

        XDestroyWindow(display, window);

        _free_cursors();
        XCloseDisplay(display);
    }

    void x11_window::set_cursor(cursor c)
    {
        XDefineCursor(display, window, x11_cursors[static_cast<int>(c)]);
    }

    bool x11_window::process_events(unsigned int usec_wait_timeout)
    {
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
                std::optional<draw_area> redraw_area;

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
        //  Notify the content that window size has changed
        resize_display(width, height);
        cairo_xlib_surface_set_size(cairo_surface, width, height);
    }

    bool x11_window::_process_event(const XEvent& event, std::optional<draw_area>& redraw_area)
    {
        switch (event.type)
        {

        case ButtonPress:
            switch (event.xbutton.button)
            {
                case 1:
                {
                    //  DBL click detection
                    const auto now = event.xbutton.time;
                    const auto delta = now - _last_click_time;

                    if (delta > 50 && delta < 250)
                        sys_mouse_dbl_click();
                    else
                        sys_mouse_button_down(mouse_button::left);

                    _last_click_time = now;
                }
                break;
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

        //  Add security pixels to be sure border are correctly redrawn
        const auto pixel_offset = 2;
        area.top -= pixel_offset;
        area.bottom += pixel_offset;
        area.left -= pixel_offset;
        area.right += pixel_offset;

        //  Compute intersection beetween area and windows (what we actually need to redraw)
        draw_area drawing_area;
        if (area.intersect(window_area, drawing_area)) {
            //  Redraw
            sys_draw_rect(cr, drawing_area.top, drawing_area.bottom, drawing_area.left, drawing_area.right);

#ifdef VIEW_DEBUG_HIGHLIGHT_REDRAW_AREA
            cairo_rectangle(cr, area.left, area.top, area.right - area.left, area.bottom - area.top);
            cairo_set_source_rgb(cr, 1, 0, 0);
            cairo_set_line_width(cr, pixel_offset);
            cairo_stroke(cr);
#endif

            XFlush(display);
        }
    }

    void x11_window::_redraw_window()
    {
        std::cout << "Redraw window" << std::endl;
        //  Redraw
        sys_draw(cr);
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
        ev.xexpose.window = window;

        XSendEvent(display, window, true, ExposureMask, &ev);
    }

    void x11_window::_initialiaze_cursors()
    {
        x11_cursors[static_cast<int>(cursor::standard)] = XCreateFontCursor(display, XC_left_ptr);
        x11_cursors[static_cast<int>(cursor::arrow)] = XCreateFontCursor(display, XC_left_ptr);
        x11_cursors[static_cast<int>(cursor::cross)] = XCreateFontCursor(display, XC_cross);
        x11_cursors[static_cast<int>(cursor::noway)] = XCreateFontCursor(display, XC_circle);
        x11_cursors[static_cast<int>(cursor::wait)] = XCreateFontCursor(display, XC_watch);
        x11_cursors[static_cast<int>(cursor::hand)] = XCreateFontCursor(display, XC_hand1);
        x11_cursors[static_cast<int>(cursor::size_horizontal)] = XCreateFontCursor(display, XC_sb_h_double_arrow);
        x11_cursors[static_cast<int>(cursor::size_vertical)] = XCreateFontCursor(display, XC_sb_v_double_arrow);
        x11_cursors[static_cast<int>(cursor::move)] = XCreateFontCursor(display, XC_fleur);
    }

    void x11_window::_free_cursors()
    {
        for (auto cur : x11_cursors)
            XFreeCursor(display, cur);
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
