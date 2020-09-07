
#include "label.h"


namespace View {

    void label::apply_color_theme(const color_theme& theme)
    {
        _color = theme.on_surface;
    }

    void label::draw(NVGcontext *vg)
    {
        nvgFillColor(vg, _color);
        draw_text(vg, 0, 0, width(), height(), _font_size, _text.c_str(), _bold, _ha, _va);
    }

    void label::set_bold(bool bold) noexcept
    {
        _bold = bold;
        invalidate();
    }

    void label::set_color(NVGcolor c) noexcept
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