#include "text_helper.h"

namespace View {

    static auto compute_txt_x(float x,float width,float text_width, horizontal_alignment align)
    {
        switch (align)
        {
            case horizontal_alignment::left:    return x;
            default:
            case horizontal_alignment::center:  return x + (width - text_width) / 2.f;
            case horizontal_alignment::right:   return x + width - text_width;
        }
    }

    static auto compute_txt_y(float y,float height,float text_height, vertical_alignment align)
    {
        switch (align)
        {
            case vertical_alignment::top:       return y + text_height;
            default:
            case vertical_alignment::center:    return y + height - (height - text_height) / 2.f;
            case vertical_alignment::bottom:    return y + height;
        }
    }

    void draw_text(
        NVGcontext *vg,
        float x, float y, float width, float height, float font_size,
        const char *txt,
        bool bold,
        horizontal_alignment ha,
        vertical_alignment va)
    {
        float bounds[4]; // left, top, right, bottom
        const float advance = nvgTextBounds(vg, 0, 0, txt, nullptr, bounds);

        auto text_x = compute_txt_x(x, width, advance, ha);
        auto text_y = compute_txt_y(y, height, bounds[3] - bounds[1], va);

        nvgFontFaceId(vg, 0);
        nvgFontSize(vg, font_size);
        nvgText(vg, text_x, text_y, txt, nullptr);
    }
}