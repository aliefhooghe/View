
#include <cstring>
#include <optional>
#include <array>
#include <chrono>
#include <iostream>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xlocale.h>

#include "display/common/display_controler.h"
#include "display/common/widget_adapter.h"
#include "x11_display.h"
#include "internal_fonts/internal_fonts.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION	// Use GL2 implementation.
#include "nanovg_gl.h"

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
         *  \brief manage the window
         *  \param running ref to a boolean value looked after to close the windows
         **/
        void process(const bool& running);

        //  display controller interface
        void set_cursor(cursor c) override;

    private:
        //  Widget adapter interface
        void sys_invalidate_rect(const draw_area& area) override;

        //  Internal helpers
        void _allocate_drawing_context(unsigned int width, unsigned int height);
        void _free_drawing_context();

        void _resize_window(unsigned int width, unsigned int height);
        bool _process_event(const XEvent& event, std::optional<draw_area>& redraw_area);    // return true if window should be closed

        void _redraw_area(draw_area area);
        void _redraw_window();

        void _initialize_cursors();
        void _free_cursors();

        //  X11 members
        Display *_display{nullptr};
        Window _window{0};
        Window _parent{0};
        Atom wm_delete_message{0};
        std::array<Cursor, VIEW_CURSOR_COUNT> x11_cursors{};

        //  dbl click detection
        Time _last_click_time{};

        //  Drawing context
        GLXContext _glx;
        NVGcontext *_vg;
    };

    x11_window::x11_window(Window parent, widget& root, float pixel_per_unit)
    :   widget_adapter{root, pixel_per_unit}
    {
        const auto width = display_width();
        const auto height = display_height();

        /** \todo check error **/
        _display = XOpenDisplay(nullptr /* display name*/);


        int attributes[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
        Window root_window = DefaultRootWindow(_display);
        XVisualInfo *visual_info =  glXChooseVisual(_display, 0, attributes);

        //  Create the windows
        const auto screen_id = DefaultScreen(_display);

        XSetWindowAttributes xattributs;
        std::memset(&xattributs, 0, sizeof(xattributs));

        //xattributs.background_pixel = BlackPixel(_display, screen_id);
        xattributs.colormap = XCreateColormap(_display, root_window, visual_info->visual, AllocNone);

        _window =
            XCreateWindow(
                _display, root_window,
                0, 0, width, height, 0,
                CopyFromParent, CopyFromParent,
                visual_info->visual,
                CWColormap, &xattributs);

        //  Reparrent the windows if a parent was given
        _parent = parent;
        if (parent != 0)
            XReparentWindow(_display, _window, parent, 0, 0);

        //  Handle windows manager close event
        wm_delete_message = XInternAtom(_display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(_display, _window, &wm_delete_message, 1);

        //  Select which type of event should be processed
        XSelectInput(_display, _window, X_EVENT_MASK);

        //  Initialize cursors
        _initialize_cursors();

        //  Map (show) the windows and wait untile it is mapped
        XMapWindow(_display, _window);
        XEvent event;
        do {
            XNextEvent(_display, &event);
        } while (event.type != MapNotify);

        //  Prepare drawing context

        //  GLX + OpenGL
        _glx = glXCreateContext(_display, visual_info, nullptr, 1);
        glXMakeCurrent(_display, _window, _glx);
        glewInit();
        glEnable(GL_STENCIL_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        //  NanoVG
        _vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

        //  Intitialize internals fonts
        create_roboto_regular_font(_vg);
        create_roboto_bold_font(_vg);

        //  Adapt windows content to the actual size
        XWindowAttributes win_attrib;
        XGetWindowAttributes(_display, _window, &win_attrib);
        _resize_window(win_attrib.width, win_attrib.height);

        //  Initial windows drawing
        _redraw_window();
    }

    x11_window::~x11_window()
    {
        nvgDeleteGL2(_vg);
        glXDestroyContext(_display, _glx);
        XDestroyWindow(_display, _window);
        _free_cursors();
        XCloseDisplay(_display);
    }

    void x11_window::set_cursor(cursor c)
    {
        XDefineCursor(_display, _window, x11_cursors[static_cast<int>(c)]);
    }

    void x11_window::process(const bool& running)
    {
        constexpr auto frame_interval = std::chrono::duration<float>{1.f/60.f};
        std::optional<draw_area> redraw_area = std::nullopt;

        auto last_draw = std::chrono::steady_clock::now();

        while (running)
        {
            //  Sleep reduce drastically cpu usage of active polling
            std::this_thread::sleep_for(std::chrono::microseconds(100));

            //  There are some event to be processed
            while (XPending(_display)) {
                XEvent event;
                XNextEvent(_display, &event);
                if (_process_event(event, redraw_area))
                    return;
            }

            //  Redraw if something need to be redrawn and a sufficient
            //  amount of time have elapsed since last redraw
            const auto now = std::chrono::steady_clock::now();
            if (redraw_area) {
                const auto current_interval = now - last_draw;

                if (current_interval >= frame_interval) {
                    _redraw_area(redraw_area.value());
                    last_draw = now;
                    redraw_area = std::nullopt;
                }
            }
        }
    }

    void x11_window::_resize_window(unsigned int width, unsigned int height)
    {
        //  Notify the content that window size has changed
        resize_display(width, height);
        //  Update drawing context
        glViewport(0, 0, width, height);
    }

    bool x11_window::_process_event(const XEvent& event, std::optional<draw_area>& redraw_area)
    {
        switch (event.type)
        {

        case ButtonPress:
            switch (event.xbutton.button)
            {
                case 1: sys_mouse_button_down(mouse_button::left); break;
                case 2: sys_mouse_button_down(mouse_button::wheel); break;
                case 3: sys_mouse_button_down(mouse_button::right); break;
                case 4: sys_mouse_wheel(1.0f);  break;
                case 5: sys_mouse_wheel(-1.f);  break;
            }
        break;

        case ButtonRelease:
        {
            switch (event.xbutton.button)
            {
                case 1:
                {
                    //  DBL click detection
                    const auto now = event.xbutton.time;
                    const auto delta = now - _last_click_time;

                    sys_mouse_button_up(mouse_button::left);
                    if (delta > 50 && delta < 250)
                        sys_mouse_dbl_click();

                    _last_click_time = now;
                    break;
                }
                case 2: sys_mouse_button_up(mouse_button::wheel); break;
                case 3: sys_mouse_button_up(mouse_button::right); break;
            }
        }
        break;

        case KeyPress:
        {
            char buffer[8];
            const auto size = XLookupString(
                const_cast<XKeyEvent*>(&event.xkey), buffer, 7, nullptr, nullptr);
            if (size > 0)
                sys_text_input(std::string_view{buffer, static_cast<std::size_t>(size)});
        }
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

        //  Do not send Mouse enter/exit event for children windows
        //  because unwanted event are received from XLib.
        /**  \todo investigate this */
        case EnterNotify:
            if (_parent == 0)
                sys_mouse_enter();
        break;

        case LeaveNotify:
            if (_parent == 0)
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
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            nvgBeginFrame(_vg, display_width(), display_height(), 1.);

            //  Redraw
            sys_draw_rect(_vg, drawing_area.top, drawing_area.bottom, drawing_area.left, drawing_area.right);

#ifdef VIEW_DEBUG_HIGHLIGHT_REDRAW_AREA
            nvgBeginPath(_vg);
            nvgRect(_vg, area.left, area.top, area.right - area.left, area.bottom - area.top);
            nvgStrokeColor(_vg, nvgRGBf(1., 0., 0.));
            nvgStrokeWidth(_vg, pixel_offset);
            nvgStroke(_vg);
#endif
            nvgEndFrame(_vg); 
            glXSwapBuffers(_display, _window);
            XFlush(_display);
        }
    }

    void x11_window::_redraw_window()
    {
        // std::cout << "Redraw window" << std::endl;
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(_vg, display_width(), display_height(), 1.);

        //  Redraw
        sys_draw(_vg);
    
        nvgEndFrame(_vg);   
        glXSwapBuffers(_display, _window);
        XFlush(_display);
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
        ev.xexpose.window = _window;

        XSendEvent(_display, _window, true, ExposureMask, &ev);
        XFlush(_display);
    }

    void x11_window::_initialize_cursors()
    {
        x11_cursors[static_cast<int>(cursor::standard)] = XCreateFontCursor(_display, XC_left_ptr);
        x11_cursors[static_cast<int>(cursor::arrow)] = XCreateFontCursor(_display, XC_left_ptr);
        x11_cursors[static_cast<int>(cursor::cross)] = XCreateFontCursor(_display, XC_cross);
        x11_cursors[static_cast<int>(cursor::noway)] = XCreateFontCursor(_display, XC_circle);
        x11_cursors[static_cast<int>(cursor::wait)] = XCreateFontCursor(_display, XC_watch);
        x11_cursors[static_cast<int>(cursor::hand)] = XCreateFontCursor(_display, XC_hand1);
        x11_cursors[static_cast<int>(cursor::size_horizontal)] = XCreateFontCursor(_display, XC_sb_h_double_arrow);
        x11_cursors[static_cast<int>(cursor::size_vertical)] = XCreateFontCursor(_display, XC_sb_v_double_arrow);
        x11_cursors[static_cast<int>(cursor::move)] = XCreateFontCursor(_display, XC_fleur);
        x11_cursors[static_cast<int>(cursor::text)] = XCreateFontCursor(_display, XC_xterm);
    }

    void x11_window::_free_cursors()
    {
        for (auto cur : x11_cursors)
            XFreeCursor(_display, cur);
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
        x11_window win{parent, self->_root, self->_pixel_per_unit};
        win.process(self->_running);
    }
}
