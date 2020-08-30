
#include "checkbox.h"
#include "drawing/shadowed.h"

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
        shadowed_down_rounded_rect(vg, 0, 0, width(), height(), 3.f, _surface);

        //  Draw border
        if (hovered()) {
            nvgStrokeColor(vg, _hovered_border);
            nvgStrokeWidth(vg, 0.5f);
            nvgStroke(vg);
        }

        if (_checked) {
            const auto unit = width() / 8.f;

            //  Draw check
            nvgBeginPath(vg);
            nvgMoveTo(vg, 2.f * unit, 5.f * unit);
            nvgLineTo(vg, 3.f * unit, 6.f * unit);
            nvgLineTo(vg, 6.f * unit, 3.f * unit);

            nvgStrokeColor(vg, _check_color);
            nvgStrokeWidth(vg, width() / 6.f);
            nvgStroke(vg);
        }
        
    }

    void checkbox::apply_color_theme(const View::color_theme& theme)
    {
        _surface = theme.surface_light;
        _hovered_border = theme.secondary_light;
        _check_color = nvgTransRGBA(theme.on_secondary, 200);
    }

    void checkbox::_switch_state()
    {
        _checked = !_checked;
        _callback(_checked);
        invalidate();
    }

}