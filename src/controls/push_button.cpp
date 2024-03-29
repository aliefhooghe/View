
#include "push_button.h"
#include "drawing/shadowed.h"

namespace View {

    push_button::push_button(float width, float height)
    : control{width, height, size_constraint::at_least(width), size_constraint::frozen(height)}
    {
    }

    push_button::push_button(
        float width,
        float height,
        size_constraint width_constraint,
        size_constraint hieght_constraint)
    : control{width, height, width_constraint, hieght_constraint}
    {
        apply_color_theme(default_color_theme);
    }

    void push_button::set_callback(callback c)
    {
        _callback = c;
    }

    bool push_button::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        if (button == mouse_button::left) {
            _update_pushed(false);
            _callback();
            return true;
        }
        else {
            return false;
        }
    }

    bool push_button::on_mouse_button_down(const mouse_button button, float x, float y)
    {
        if (button == mouse_button::left) {
            _update_pushed(true);
            return true;
        }
        else {
            return false;
        }
    }

    bool push_button::on_mouse_exit()
    {
        control::on_mouse_exit();
        _update_pushed(false);
        return true;
    }

    bool push_button::on_mouse_drag_cancel()
    {
        _update_pushed(false);
        return true;
    }

    void push_button::draw(NVGcontext *vg)
    {
        //  Background
        if (_pushed) {
            shadowed_down_rounded_rect(vg, 0, 0, width(), height(), 3.f, _surface);
        }
        else {
            shadowed_up_rounded_rect(vg, 0, 0, width(), height(), 3.f, _surface);
        }

        if (hovered()) {
            nvgFillColor(vg, _hovered_border);
            nvgFill(vg);
        }
    }

    void push_button::apply_color_theme(const View::color_theme& theme)
    {
        _hovered_border = nvgTransRGBA(theme.secondary_light, 48);
        _surface = theme.surface_light;
        invalidate();
    }

    void push_button::_update_pushed(bool pushed)
    {
        if (pushed != _pushed) {
            _pushed = pushed;
            invalidate();
        }
    }

}