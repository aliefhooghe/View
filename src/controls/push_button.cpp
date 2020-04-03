
#include "push_button.h"
#include "drawing/cairo_helper.h"
#include <iostream>

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
        std::cout << "Push button drag cancel" << std::endl;
        _update_pushed(false);
        return true;
    }

    void push_button::draw(cairo_t *cr)
    {
        rounded_rectangle(cr, 0, 0, width(), height(), 0.3f);

        //  Draw background
        set_source(cr, _background_color);

        if (hovered()) {
            cairo_fill_preserve(cr);

            //  Draw border
            set_source(cr, _hovered_border_color);
            cairo_set_line_width(cr, _pushed ? 0.3f : 0.2f);
            cairo_stroke(cr);
        }
        else {
            cairo_fill(cr);
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