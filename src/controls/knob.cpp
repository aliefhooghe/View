
#include <cmath>
#include <algorithm>

#include "knob.h"
#include "drawing/cairo_helper.h"
#include "drawing/named_colors.h"

namespace View {

    knob::knob(float size, float initial_value)
    :   control{size, size}, _value{initial_value}
    {}

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
        set_value(_value + 0.05f * distance);
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
        set_source(cr, hovered() ? 0x45A1FFFF : named_colors::gray);
        cairo_set_line_width(cr, 0.3);
        cairo_stroke(cr);

        //  Light Indicator
        const auto cur_angle = start_angle + _value * delta_angle;

        cairo_arc(cr, half, half, 0.75f * half, start_angle, cur_angle);
        set_source(cr, 0x45A1FFFF);
        cairo_set_line_width(cr, 0.5);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
        cairo_stroke(cr);
    }
}