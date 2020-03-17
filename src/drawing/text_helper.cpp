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
        cairo_t *cr,
        float x, float y, float width, float height, float font_size,
        const char *txt,
        bool bold,
        horizontal_alignment ha,
        vertical_alignment va)
    {
        cairo_text_extents_t te;

        cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, font_size);
        cairo_text_extents(cr, txt, &te);

        auto text_x = compute_txt_x(x, width, te.width, ha);
        auto text_y = compute_txt_y(y, height, te.height, va);

        cairo_new_sub_path(cr);
        cairo_move_to(cr, text_x, text_y);

        cairo_show_text(cr, txt);
        cairo_close_path(cr);
    }
}