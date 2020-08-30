
#include "shadowed.h"

namespace View {
    
    void shadowed_down_rounded_rect(
        NVGcontext *vg,
        float x, float y, float width, float height, float radius,
        NVGcolor background, NVGcolor surface)
    {
        const auto shadow = nvgRGB(0, 0, 0);
        const auto gradient = 
            nvgBoxGradient(vg, x + 1.5f, y + 1.8f, width - 2.f, height- 2.f, radius, 2.f, surface, shadow);
    
        //  Draw background (with shadow)
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, width, height, radius);
        nvgFillPaint(vg, gradient);
        nvgFill(vg);
    }

    void shadowed_up_rounded_rect(
        NVGcontext *vg,
        float x, float y, float width, float height, float radius,
        NVGcolor background, NVGcolor surface)
    {
        const auto shadow = nvgRGB(0, 0, 0);
        // const auto gradient = 
        //     nvgBoxGradient(vg, x, y, width + 1.f, height + 1.2f, radius, 2.f, shadow, background);

        const auto gradient = 
            nvgBoxGradient(vg, x + 0.5f, y + 0.5f, width + 1.f, height + 1.3f, radius, 2.f, shadow, background);

        //  Draw shadow
        nvgBeginPath(vg);
        nvgRect(vg, x - 1.f, y - 1.f, width + 4.f, height + 4.f);
        nvgFillPaint(vg, gradient);
        nvgFill(vg);

        //  Draw background
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, width, height, radius);
        nvgFillColor(vg, surface);
        nvgFill(vg);
    }

}