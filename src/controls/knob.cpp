
#include <cmath>
#include <algorithm>
#include <cstring>

#include "knob.h"
#include "drawing/text_helper.h"
#include "drawing/shadowed.h"

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

    void knob::draw(NVGcontext *vg)
    {
        constexpr auto theta = 0.5f;

        constexpr auto start_angle = M_PI_2 + theta;
        constexpr auto delta_angle = 2.f * (M_PI - theta);

        const auto half = width() / 2.f;

        //  External Circle + background
        shadowed_up_circle(vg, half, half, half, _background, _surface);
        
        if (hovered()) {
            //  Draw Border
            nvgStrokeColor(vg, _hovered_border);
            nvgStrokeWidth(vg, 0.5f);
            nvgStroke(vg);
        }

        //  Light Indicator
        const auto cur_angle = start_angle + _value * delta_angle;

        nvgBeginPath(vg);
        nvgArc(vg, half, half, 0.75f * half, start_angle, cur_angle, NVG_CW);
        nvgStrokeColor(vg, _track);
        nvgStrokeWidth(vg, 7.f);
        nvgStroke(vg);

        //  Value text
        nvgFillColor(vg, _text);
        if (_display_value) {
            char text[5];
            std::snprintf(text, sizeof(text), "%.02f", _value);
            draw_text(
                vg, half/2, 0, width(), height(), 14.f, text, false, horizontal_alignment::none);
        }
    }

    void knob::apply_color_theme(const View::color_theme& theme)
    {
        _background = theme.surface_dark;
        _surface = theme.surface_light;
        _track = nvgTransRGBAf(theme.secondary, 0.4f);
        _hovered_border = theme.secondary_light;
        _text = theme.on_surface;
        invalidate();
    }
}