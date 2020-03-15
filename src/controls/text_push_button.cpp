
#include "text_push_button.h"
#include "drawing/text_helper.h"

namespace View {

    void text_push_button::draw(cairo_t *cr)
    {
        push_button::draw(cr);
        set_source(cr, _text_color);
        draw_text(cr, 0, 0, width(), height(), 2.5f, _text.c_str(), true);
    }

    void text_push_button::apply_color_theme(const View::color_theme &theme)
    {
        push_button::apply_color_theme(theme);
        _text_color = theme.on_surface;
    }

}