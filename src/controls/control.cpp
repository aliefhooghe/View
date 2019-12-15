
#include "control.h"
#include <iostream>

namespace View {

    control::control(float width, float height)
    : widget{width, height}
    {
    }

    bool control::on_mouse_enter()
    {
        _hovered = true;
        set_cursor(cursor::hand);
        invalidate();
        return true;
    }

    bool control::on_mouse_exit()
    {
        _hovered = false;
        set_cursor(cursor::standard);
        invalidate();
        return true;
    }

    bool control::hovered() const noexcept
    {
        return _hovered;
    }

}