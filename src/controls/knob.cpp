
#include <cmath>
#include <algorithm>
#include <cstring>

#include "knob.h"
#include "drawing/cairo_helper.h"
#include "drawing/text_helper.h"

namespace View {

    knob::knob(float size, float initial_value, bool display_value)
    :   control{size, size}, _value{initial_value}, _display_value{display_value}
    {
        apply_color_theme(default_color_theme);
    }

    void knob::set_callback(callback c)
    {
        _callback = c;
    }

    void knob::set_value(float value)
    {
        _value = std::clamp(value, 0.f, 1.f);
        _callback(_value);
        invalidate();
    }

    bool knob::on_mouse_drag(mouse_button, float, float, float, float dy)
    {
        set_value(_value - 0.1f * dy);
        return true;
    }

    bool knob::on_mouse_wheel(float distance)
    {
        set_value(_value + 0.02f * distance);
        return true;
    }

    void knob::draw(cairo_t *cr)
    {
        constexpr auto theta = 0.5f;

        constexpr auto start_angle = M_PI_2 + theta;
        constexpr auto delta_angle = 2.f * (M_PI - theta);

        const auto half = width() / 2.f;

        //  External Circle
        circle(cr, half, half, half);
        set_source(cr, hovered() ? _hovered_border_color: _border_color);
        cairo_set_line_width(cr, width() / 30.f);
        cairo_stroke(cr);

        //  Light Indicator
        const auto cur_angle = start_angle + _value * delta_angle;

        cairo_arc(cr, half, half, 0.75f * half, start_angle, cur_angle);
        set_source(cr, _track_color);
        cairo_set_line_width(cr, width() / 6.f);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
        cairo_stroke(cr);

        //  Value text
        if (_display_value) {
            char text[5];
            std::snprintf(text, sizeof(text), "%.2f", _value);
            draw_centered_text(cr, 0, 0, width(), height(), height() / 5.f, text, _text_color, true);
        }
    }

    void knob::apply_color_theme(const View::color_theme& theme)
    {
        _track_color = theme.secondary_dark;
        _border_color = theme.on_surface;
        _hovered_border_color = theme.secondary;
        _text_color = theme.on_surface;
        invalidate();
    }
}