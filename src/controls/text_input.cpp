
#include <cctype>
#include <iostream>

#include "text_input.h"
#include "drawing/text_helper.h"
#include "drawing/shadowed.h"

namespace View {

    text_input::text_input(float width, float height)
    :   control{
            width, height,
            size_constraint::at_least(width),
            size_constraint::frozen(height), cursor::text}
    {
        apply_color_theme(default_color_theme);
    }

    text_input::text_input(
        float width, float height,
        size_constraint width_constraint,
        size_constraint height_constraint)
    :   control{width, height, width_constraint, height_constraint, cursor::text}
    {
        apply_color_theme(default_color_theme);
    }

    bool text_input::on_char_input(char c)
    {
        //  ascii backspace
        if (c == 8u) {
            if (!_text.empty())
                _text.pop_back();
        }
        else if (c == 13) {
            _enter_callback();
        }
        else if (std::isprint(c)) {
            _text.append(1, c);
        }

        invalidate();
        return true;
    }

    void text_input::draw(NVGcontext *vg)
    {
        //  Background
        shadowed_down_rounded_rect(vg, 0, 0, width(), height(), 3.f, _surface_color);

        //  Border
        if (hovered()) {
            nvgFillColor(vg, _hoverred_border_color);
            nvgFill(vg);
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
        _surface_color = theme.surface_light;
        _hoverred_border_color = nvgTransRGBA(theme.secondary_light, 48);
    }

    void text_input::clear_text()
    {
        _text = "";
        invalidate();
    }

    void text_input::set_text(const std::string& txt)
    {
        _text = txt;
        invalidate();
    }

    void text_input::set_enter_callback(callback enter_callback)
    {
        _enter_callback = enter_callback;
    }

}