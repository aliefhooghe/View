#include "text_helper.h"

namespace View {

    static auto compute_txt_x_offset(float width, const float bounds[4], horizontal_alignment align)
    {
        const auto text_width = bounds[2];// - bounds[0];
        switch (align)
        {
            case horizontal_alignment::left:    return 0.f;
            default:
            case horizontal_alignment::center:  return (width - text_width) / 2.f;
            case horizontal_alignment::right:   return width - text_width;
            case horizontal_alignment::none:    return 0.f;
        }
    }

    static auto compute_txt_y_offset(float height, const float bounds[4], vertical_alignment align)
    {
        switch (align)
        {
            case vertical_alignment::top:       return -bounds[1];
            default:
            case vertical_alignment::center:    return (height - (bounds[1] + bounds[3])) / 2.f;
            case vertical_alignment::bottom:    return (height - bounds[3]);
            case vertical_alignment::none:      return height;
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

        //  Set font
        //nvgFontFaceId(vg, 0);
        nvgFontSize(vg, static_cast<int>(font_size));

        //  Compute pos
        nvgTextBounds(vg, 0, 0, txt, nullptr, bounds);
        auto text_x_offset = compute_txt_x_offset(width, bounds, ha);
        auto text_y_offset = compute_txt_y_offset(height, bounds, va);

        //  Draw text
        nvgText(
            vg, static_cast<int>(x + text_x_offset),
            static_cast<int>(y + text_y_offset),
            txt, nullptr);
    }
}