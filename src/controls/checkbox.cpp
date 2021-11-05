
#include "checkbox.h"
#include "drawing/shadowed.h"

namespace View {

    checkbox::checkbox(float size, bool checked)
    : control{
        size, size,
        size_constraint::frozen(size),
        size_constraint::frozen(size)
     }, _checked{checked}
    {
        apply_color_theme(default_color_theme);
    }

    void checkbox::set_checked(bool checked)
    {
        if (_checked != checked) {
            _checked = checked;
            invalidate();
        }
    }

    bool checkbox::on_mouse_button_up(mouse_button button, float x, float y)
    {
        _switch_state();
        return true;
    }

    void checkbox::draw(NVGcontext *vg)
    {
        shadowed_down_rounded_rect(vg, 0, 0, width(), height(), 3.f, _surface);

        if (hovered()) {
            nvgFillColor(vg, _hovered_color);
            nvgFill(vg);
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
        _hovered_color = nvgTransRGBA(theme.secondary_light, 48);
        _check_color = nvgTransRGBA(theme.secondary, 200);
    }

    void checkbox::_switch_state()
    {
        _checked = !_checked;
        _callback(_checked);
        invalidate();
    }

}