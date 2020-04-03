
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
        set_value(_value - 0.02f * dy);
        return true;
    }

    bool knob::on_mouse_wheel(float distance)
    {
        set_value(_value + 0.01f * distance);
        return true;
    }

    void knob::draw(cairo_t *cr)
    {
        constexpr auto theta = 0.5f;

        constexpr auto start_angle = M_PI_2 + theta;
        constexpr auto delta_angle = 2.f * (M_PI - theta);

        const auto half = width() / 2.f;

        //  External Circle + background
        circle(cr, half, half, half);

        set_source(cr, _background_color);
        cairo_fill_preserve(cr);

        set_source(cr, hovered() ? _hovered_border_color: _border_color);
        cairo_set_line_width(cr, 0.2f);
        cairo_stroke(cr);

        //  Light Indicator
        const auto cur_angle = start_angle + _value * delta_angle;

        cairo_arc(cr, half, half, 0.75f * half, start_angle, cur_angle);
        set_source(cr, _track_color);
        cairo_set_line_width(cr, 0.5f);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
        cairo_stroke(cr);

        //  Value text
        if (_display_value) {
            char text[5];
            std::snprintf(text, sizeof(text), "%.2f", _value);
            set_source(cr, _text_color);
            draw_text(
                cr, 0, 0, width(), height(), 1.f, text);
        }
    }

    void knob::apply_color_theme(const View::color_theme& theme)
    {
        _background_color = theme.surface_light;
        _track_color = theme.secondary;
        _border_color = theme.surface_light;
        _hovered_border_color = theme.secondary_light;
        _text_color = theme.on_surface;
        invalidate();
    }
}