
#include "text_push_button.h"
#include "drawing/text_helper.h"

namespace View {

    void text_push_button::draw(NVGcontext *vg)
    {
        push_button::draw(vg);
        nvgFillColor(vg, _text_color);
        draw_text(vg, 0, 0, width(), height(), 14.0f, _text.c_str());
    }

    void text_push_button::apply_color_theme(const View::color_theme &theme)
    {
        push_button::apply_color_theme(theme);
        _text_color = theme.on_surface;
    }

}