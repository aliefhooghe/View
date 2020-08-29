
#include "checkbox.h"

namespace View {

    checkbox::checkbox(float size, bool checked)
    : control{size, size}, _checked{checked}
    {
        apply_color_theme(default_color_theme);
    }

    bool checkbox::on_mouse_button_up(mouse_button button, float x, float y)
    {
        _switch_state();
        return true;
    }

    void checkbox::draw(NVGcontext *vg)
    {
        const auto unit = width() / 8.f;

        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, width(), height(), unit);

        if (_checked) {
            //  Draw background
            nvgFillColor(vg, _background);
            nvgFill(vg);

            //  Draw check
            nvgBeginPath(vg);
            nvgMoveTo(vg, 2.f * unit, 5.f * unit);
            nvgLineTo(vg, 3.f * unit, 6.f * unit);
            nvgLineTo(vg, 6.f * unit, 3.f * unit);

            nvgStrokeColor(vg, _check_color);
            nvgStrokeWidth(vg, width() / 6.f);
            nvgStroke(vg);
        }
        else {
            //  Draw border
            nvgStrokeColor(vg, hovered() ? _hovered_color : _border);
            nvgStrokeWidth(vg, width() / 10.f);
            nvgStroke(vg);
        }

    }

    void checkbox::apply_color_theme(const View::color_theme& theme)
    {
        _background = theme.secondary_dark;
        _border = theme.surface_light;
        _hovered_color = theme.secondary_light;
        _check_color = theme.on_secondary;
    }

    void checkbox::_switch_state()
    {
        _checked = !_checked;
        _callback(_checked);
        invalidate();
    }

}