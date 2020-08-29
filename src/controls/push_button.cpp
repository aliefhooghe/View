
#include "push_button.h"

namespace View {

    push_button::push_button(float width, float height)
    : control{width, height}
    {
        apply_color_theme(default_color_theme);
    }

    void push_button::set_callback(callback c)
    {
        _callback = c;
    }

    bool push_button::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        _update_pushed(false);
        _callback();
        return true;
    }

    bool push_button::on_mouse_button_down(const mouse_button button, float x, float y)
    {
        _update_pushed(true);
        return true;
    }

    bool push_button::on_mouse_drag_cancel()
    {
        _update_pushed(false);
        return true;
    }

    void push_button::draw(NVGcontext *vg)
    {
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, width(), height(), 4.2f);

        //  Draw background
        nvgFillColor(vg, _background_color);
        nvgFill(vg);

        if (hovered()) {
            //  Draw border
            nvgStrokeColor(vg, _hovered_border_color);
            nvgStrokeWidth(vg, _pushed ? 4.2f : 2.8f);
            nvgStroke(vg);
        }
    }

    void push_button::apply_color_theme(const View::color_theme& theme)
    {
        _border_color = theme.on_surface;
        _hovered_border_color = theme.secondary_light;
        _background_color = theme.surface_light;
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