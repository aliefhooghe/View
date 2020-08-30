
#include <cctype>
#include <iostream>

#include "text_input.h"
#include "drawing/text_helper.h"
#include "drawing/shadowed.h"

namespace View {

    text_input::text_input(float width, float height)
    :   control{width, height, cursor::text}
    {
        apply_color_theme(default_color_theme);
    }

    bool text_input::on_text_input(const std::string_view txt)
    {
        for (const auto c : txt) {
            //  ascii backspace
            if (c == 8u) {
                if (!_text.empty())
                    _text.pop_back();
            }
            else if (c == 13) {
                std::cout << "enter!" << std::endl;
            }
            else if (std::isprint(c)) {
                _text.append(1, c);
            }
        }

        invalidate();
        return true;
    }

    void text_input::draw(NVGcontext *vg)
    {
        //  Background
        shadowed_down_rounded_rect(vg, 0, 0, width(), height(), 3.f, _background_color, _surface_color);

        //  Border
        if (hovered()) {
            nvgStrokeWidth(vg, 0.5f);
            nvgStrokeColor(vg, _hoverred_border_color);
            nvgStroke(vg);
        }

        //  Draw text
        nvgIntersectScissor(vg, 0, 0, width(), height());
        nvgFillColor(vg, _text_color);
        draw_text(vg, 0, 0, width() - 1.f, height(), 14.f, _text.c_str(), false,
            horizontal_alignment::right);
    }

    void text_input::apply_color_theme(const color_theme& theme)
    {
        _text_color = theme.on_surface;
        _background_color = theme.surface_dark;
        _surface_color = theme.surface_light;
        _hoverred_border_color = theme.secondary_light;
    }

    void text_input::clear_text()
    {
        _text = "";
        invalidate();
    }

}