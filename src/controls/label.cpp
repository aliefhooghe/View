
#include "label.h"


namespace View {

    void label::apply_color_theme(const color_theme& theme)
    {
        _color = theme.on_surface;
    }

    void label::draw(cairo_t *cr)
    {
        set_source(cr, _color);
        draw_text(cr, 0, 0, width(), height(), _font_size, _text.c_str(), _bold, _ha, _va);
    }

    void label::set_bold(bool bold) noexcept
    {
        _bold = bold;
        invalidate();
    }

    void label::set_color(color c) noexcept
    {
        _color = c;
        invalidate();
    }

    void label::set_font_size(float size) noexcept
    {
        _font_size = size;
        invalidate();
    }

}