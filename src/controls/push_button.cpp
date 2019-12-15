
#include "push_button.h"
#include "drawing/color.h"
#include "drawing/cairo_helper.h"
#include "drawing/named_colors.h"

#include <iostream>

namespace View {

    push_button::push_button(float width, float heigh)
    : control{width, heigh}
    {
    }

    void push_button::set_callback(callback c)
    {
        _callback = c;
    }

    bool push_button::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        _update_pushed(false);
        _callback(*this);
        return true;
    }

    bool push_button::on_mouse_button_down(const mouse_button button, float x, float y)
    {
        _update_pushed(true);
        return true;
    }

    bool push_button::on_mouse_drag_end(const mouse_button button, float x, float y)
    {
        std::cout << "Push button drag end" << std::endl;
        _update_pushed(false);

        if (contains(x, y))
            _callback(*this);

        return true;
    }

    bool push_button::on_mouse_drag_cancel()
    {
        std::cout << "Push button drag cancel" << std::endl;
        _update_pushed(false);
        return true;
    }

    void push_button::draw(cairo_t* cr)
    {
        rounded_rectangle(cr, 0, 0, width(), height(), 0.3);

        //  Draw background
        set_source(cr, _pushed ? 0x525252FF : 0x424242FF);
        cairo_fill_preserve(cr);

        //  Draw border
        set_source(cr, hovered() ? 0x45A1FFFF: named_colors::gray);
        cairo_set_line_width(cr, 0.3);
        cairo_stroke(cr);
    }

    void push_button::_update_pushed(bool pushed)
    {
        if (pushed != _pushed) {
            _pushed = pushed;
            invalidate();
        }
    }

}