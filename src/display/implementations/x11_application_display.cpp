
#include "x11_application_display.h"

namespace View {

    x11_application_display::x11_application_display(widget& root, float pixel_per_unit)
    :    x11_display{root, pixel_per_unit}
    {
    }

    x11_application_display::~x11_application_display()
    {
        if (is_open())
            close();
    }

    void x11_application_display::open(const std::string& title)
    {
        /** TODO TITLE **/
        _create_window();
    }
    void x11_application_display::wait()
    {
        _wait_window_thread();
    }

    void x11_application_display::close()
    {
        _close_window();
        _wait_window_thread();
    }

    bool x11_application_display::is_open()
    {
        return _is_open();
    }

}