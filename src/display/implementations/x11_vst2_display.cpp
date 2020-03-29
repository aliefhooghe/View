
#include "x11_vst2_display.h"

namespace View {

    x11_vst2_display::x11_vst2_display(widget &root, float pixel_per_unit)
    :   x11_display{root, pixel_per_unit}
    {
    }

    void x11_vst2_display::open(void *parent_handle)
    {
        _create_window(reinterpret_cast<Window>(parent_handle));
    }

    void x11_vst2_display::close()
    {
        x11_display::_close_window();
        _wait_window_thread();
    }
}