
#include <algorithm>

#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <array>

#include "nanovg.h"
#include "nanovg_gl.h"

#include "win32_backend.h"

#include "display/common/display_controler.h"
#include "display/common/widget_adapter.h"
#include "internal_fonts/internal_fonts.h"

#define WINDOW_VIEW_CLASS_NAME "ViewWindow"

namespace View {

    class win32_window : private widget_adapter {
    public:
        win32_window(
            widget& root, float pixel_per_unit,
            const std::string& title, HWND parent = 0);
        win32_window(const win32_window&) = delete;
        ~win32_window();

        void manage_event_loop(const bool& running);

        // display controller interface
        void set_cursor(cursor c) override;

        using widget_adapter::sys_char_input;
    private:
        //  Widget adapter interface
        void sys_invalidate_rect(const draw_area& area) override;

        //  Internal helpers
        void _redraw_window();
        void _resize_content(unsigned int width, unsigned int height);
        void _initialize_cursors();
        void _apply_cursor();

        static void _register_window_class();

        static LRESULT CALLBACK _handle_event(
            HWND window,
            UINT msg,
            WPARAM w_param,
            LPARAM l_param);

        // Drawing context
        NVGcontext* _vg;
        HGLRC _opengl_context{};

        // Win32 members
        cursor _current_cursor{cursor::standard};
        std::array<HCURSOR, VIEW_CURSOR_COUNT> _win32_cursors{};
        const HWND _parent;
        HWND _window{0};
        bool _has_focus{false};

    };

    /**
     * in32 window implementation
     */

    win32_window::win32_window(widget& root, float pixel_per_unit, const std::string& title, HWND parent)
    :   widget_adapter{root, pixel_per_unit},
        _parent{parent}
    {
        DWORD window_style = WS_VISIBLE;
        const auto window_width = display_width();
        const auto window_height = display_height();
        auto window_x = 0u;
        auto window_y = 0u;

        // To be done in event loop thread
        _register_window_class();

        // If this is a root window
        if (parent == 0) {
            // take care of position
            POINT cursor_pos;
            GetCursorPos(&cursor_pos);

            window_x = static_cast<unsigned int>(
                    std::max<int>(0, cursor_pos.x - static_cast<int>(window_width / 2)));
            window_y = static_cast<unsigned int>(
                    std::max<int>(0, cursor_pos.y - static_cast<int>(window_height / 2)));

            //  add menu icons
            window_style |= WS_OVERLAPPEDWINDOW | CS_VREDRAW | CS_HREDRAW;
        }
        else {
            window_style |= WS_CHILDWINDOW;
        }

        // Create the window
        _window = CreateWindowA(
            WINDOW_VIEW_CLASS_NAME,
            title.c_str(), window_style,
            window_x, window_y, window_width, window_height,
            parent, nullptr, nullptr, this);

        if (_window == nullptr)
            throw std::runtime_error("win32_backend : unable to create windows");

        // Initialize the drawing context
        auto hdc = GetDC(_window);
        PIXELFORMATDESCRIPTOR pixel_format_descriptor;

        std::memset(&pixel_format_descriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));

        pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pixel_format_descriptor.nVersion = 1;
        pixel_format_descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
        pixel_format_descriptor.cColorBits = 32;

        auto pixel_format = ChoosePixelFormat(hdc, &pixel_format_descriptor);

        if (pixel_format == 0)
            throw std::runtime_error("win32_backend : unable to find a pixel format");

        if (!SetPixelFormat(hdc, pixel_format, &pixel_format_descriptor))
            throw std::runtime_error("win32_backend : unable to set pixel format");

        DescribePixelFormat(hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor);

        _opengl_context = wglCreateContext(hdc);
        wglMakeCurrent(hdc, _opengl_context);

        // Glew + OpenGL
        glewInit();
        glEnable(GL_STENCIL_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        //  NanoVG
        _vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

        //  Intitialize internals fonts and cursors
        create_roboto_regular_font(_vg);
        create_roboto_bold_font(_vg);

        _initialize_cursors();
        _apply_cursor();

        ReleaseDC(_window, hdc);
    }

    win32_window::~win32_window()
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(_opengl_context);
        DestroyWindow(_window);
    }

    void win32_window::manage_event_loop(const bool& running)
    {
        SetFocus(_window);

        while (running) {
            MSG msg;
            const auto got_msg = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
            
            if (got_msg) {
                if (msg.message != WM_QUIT) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else {
                    break;
                }
            }
        }
    }

    void win32_window::set_cursor(cursor c)
    {
        _current_cursor = c;
        _apply_cursor();
    }

    void win32_window::sys_invalidate_rect(const draw_area& area)
    {
        const RECT win32_rect =
        {
            (LONG)area.left,
            (LONG)area.top,
            (LONG)area.right,
            (LONG)area.bottom
        };

        InvalidateRect(_window, &win32_rect, true);
    }

    void win32_window::_redraw_window()
    {
        const auto window_area = make_rectangle(0, display_height(), 0, display_width());

        PAINTSTRUCT paint_struct{};
        BeginPaint(_window, &paint_struct);

        // Contains the area to be redrawn
        const draw_area rc_paint{
            paint_struct.rcPaint.top,
            paint_struct.rcPaint.bottom,
            paint_struct.rcPaint.left,
            paint_struct.rcPaint.right };

        draw_area drawing_area;
        if (rc_paint.intersect(window_area, drawing_area)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            nvgBeginFrame(_vg, display_width(), display_height(), 1.);

            sys_draw_rect(_vg, drawing_area.top, drawing_area.bottom, drawing_area.left, drawing_area.right);

            nvgEndFrame(_vg);
            glFlush();
            SwapBuffers(paint_struct.hdc);
        }

        EndPaint(_window, &paint_struct);
    }

    void win32_window::_resize_content(unsigned int width, unsigned int height)
    {
        //  Notify the content that window size has changed
        resize_display(width, height);
        //  Update drawing context
        glViewport(0, 0, width, height);
    }

    void win32_window::_initialize_cursors()
    {
        _win32_cursors[static_cast<int>(cursor::standard)] = LoadCursor(nullptr, IDC_ARROW);
        _win32_cursors[static_cast<int>(cursor::arrow)] = LoadCursor(nullptr, IDC_ARROW);
        _win32_cursors[static_cast<int>(cursor::cross)] = LoadCursor(nullptr, IDC_CROSS);
        _win32_cursors[static_cast<int>(cursor::noway)] = LoadCursor(nullptr, IDC_NO);
        _win32_cursors[static_cast<int>(cursor::wait)] = LoadCursor(nullptr, IDC_WAIT);
        _win32_cursors[static_cast<int>(cursor::hand)] = LoadCursor(nullptr, IDC_HAND);
        _win32_cursors[static_cast<int>(cursor::size_horizontal)] = LoadCursor(nullptr, IDC_SIZEWE);
        _win32_cursors[static_cast<int>(cursor::size_vertical)] = LoadCursor(nullptr, IDC_SIZENS);
        _win32_cursors[static_cast<int>(cursor::move)] = LoadCursor(nullptr, IDC_SIZEALL);
        _win32_cursors[static_cast<int>(cursor::text)] = LoadCursor(nullptr, IDC_IBEAM);
    }

    void win32_window::_apply_cursor()
    {
        const auto cursor_idx = static_cast<unsigned int>(_current_cursor);
        if (cursor_idx < VIEW_CURSOR_COUNT)
            SetCursor(_win32_cursors[cursor_idx]);
    }

    void win32_window::_register_window_class()
    {
        static bool have_been_registered = false;

        if (!have_been_registered) {
            WNDCLASS window_class;

            std::memset(&window_class, 0, sizeof(WNDCLASS));

            window_class.style = CS_DBLCLKS;
            window_class.lpfnWndProc = _handle_event;
            window_class.cbClsExtra = 0;
            window_class.cbWndExtra = 0;
            window_class.hInstance = nullptr;
            window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            window_class.hCursor = nullptr; // let the windows choose
            window_class.hbrBackground = nullptr;
            window_class.lpszMenuName = nullptr;
            window_class.lpszClassName = TEXT(WINDOW_VIEW_CLASS_NAME);

            RegisterClass(&window_class);
            have_been_registered = true;
        }
    }

    LRESULT CALLBACK win32_window::_handle_event(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
    {
        auto window_instance = reinterpret_cast<win32_window*>(GetWindowLongPtr(window, GWLP_USERDATA));

        switch (msg) {

        case WM_CREATE:
        {
            //  Save win32_window instance pointer
            auto create_struct = reinterpret_cast<LPCREATESTRUCT>(l_param);
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LPARAM>(create_struct->lpCreateParams));
        }
        break;

        case WM_PAINT:
        {
            window_instance->_redraw_window();
        }
        break;

        case WM_SIZE:
            window_instance->_resize_content(LOWORD(l_param), HIWORD(l_param));
            break;

        case WM_MOVE:
            InvalidateRect(window_instance->_window, NULL, TRUE);
            break;

        case WM_MOUSEMOVE:
        {
            if (!window_instance->_has_focus) {
                // Mouse enter : ask for a leave notification
                tagTRACKMOUSEEVENT track_mouse_event_param;
                track_mouse_event_param.cbSize = sizeof(tagTRACKMOUSEEVENT);
                track_mouse_event_param.dwFlags = TME_LEAVE;
                track_mouse_event_param.hwndTrack = window;
                TrackMouseEvent(&track_mouse_event_param);
                window_instance->sys_mouse_enter();
                window_instance->_has_focus = true;

                // Make sure to re apply cursor when the mouse enter
                window_instance->_apply_cursor();
            }

            window_instance->sys_mouse_move(
                GET_X_LPARAM(l_param),
                GET_Y_LPARAM(l_param));
        }
        break;

        case WM_MOUSEWHEEL:
        {
            const auto delta = GET_WHEEL_DELTA_WPARAM(w_param);
            window_instance->sys_mouse_wheel(delta < 0 ? -1.f : 1.f);
        }
        break;

        case WM_MOUSELEAVE:
            window_instance->sys_mouse_exit();
            window_instance->_has_focus = false;
            break;

        case WM_LBUTTONDOWN:
            window_instance->sys_mouse_button_down(mouse_button::left);
            break;

        case WM_RBUTTONDOWN:
            window_instance->sys_mouse_button_down(mouse_button::right);
            break;

        case WM_LBUTTONUP:
            window_instance->sys_mouse_button_up(mouse_button::left);
            break;

        case WM_RBUTTONUP:
            window_instance->sys_mouse_button_up(mouse_button::right);
            break;

        case WM_LBUTTONDBLCLK:
            window_instance->sys_mouse_dbl_click();
            break;

        case WM_CHAR:
        {
            window_instance->sys_char_input(
                static_cast<char>(w_param));
        }
            break;

        case WM_CLOSE:
            if (window_instance->_parent == nullptr)
            {
                // Only post a quit message if this is the root windows
                PostQuitMessage(0);
            }
            break;

        default:
            return DefWindowProc(window, msg, w_param, l_param);
            break;
        }

        return 0;
    }

    /**
     * Win32 backend implementation
     */

    win32_backend::win32_backend(widget& root, float pixel_per_unit)
        : view_backend{root, pixel_per_unit}
    {

    }

    void win32_backend::create_window(const std::string& title, void* parent)
    {
        if (_running == false) {
            _running = true;

            if (parent == nullptr) {
                // Root window : need a thread to manage the window
                _window_thread = std::thread{ _app_window_proc, this, title};

            }
            else {
                // children window : event are manager by parent
                _window = std::make_unique<win32_window>(
                    _root, _pixel_per_unit, title, reinterpret_cast<HWND>(parent));
            }
        }
    }

    void win32_backend::wait_window_thread()
    {
        if (_window_thread.joinable())
            _window_thread.join();
    }

    void win32_backend::close_window()
    {
        if (_running == true) {
            _running = false;

            if (_window_thread.joinable()) {
                // instance is deleted by the window thread
                _window_thread.join();
            }
            else {
                // there is no event manager, so we must delete the window instance
                _window.reset();
            }
        }
    }

    bool win32_backend::windows_is_open() const noexcept
    {
        return _running;
    }

    bool win32_backend::vst2_char_input(char c)
    {
        // Apply only on a child windows (for audio plugins)
        if (!_window_thread.joinable() && _window)
            return _window->sys_char_input(c);
        else
            return false;
    }

    void win32_backend::_app_window_proc(win32_backend* self, const std::string& title)
    {
        // Window must be create, used and deleted in the same thread
        self->_window = std::make_unique<win32_window>(self->_root, self->_pixel_per_unit, title);

        //  Manage event until windows is closed
        self->_window->manage_event_loop(self->_running);

        //  Destroy window
        self->_window.reset();
    }

} /* View */