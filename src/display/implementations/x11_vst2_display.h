#ifndef VIEW_X11_VST_DISPLAY_H_
#define VIEW_X11_VST_DISPLAY_H_

#include "../backends/x11_display.h"
#include "display/display_types/vst2_display.h"

namespace View {

    class x11_vst2_display : public vst2_display, private x11_display {
    public:
        x11_vst2_display(widget &root, float pixel_per_unit);

        void open(void *parent_handle) override;
        void close() override;
        unsigned int px_width() override { return x11_display::px_width(); }
        unsigned int px_height() override { return x11_display::px_height(); }
    };

}

#endif