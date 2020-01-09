
#include "control.h"
#include <iostream>

namespace View {

    control::control(float width, float height, cursor c, bool redraw_when_hoverred)
    : widget{width, height},
        _cursor{c},
        _redraw_when_hoverred{redraw_when_hoverred}
    {
    }

    bool control::on_mouse_enter()
    {
        _hovered = true;
        set_cursor(_cursor);
        if (_redraw_when_hoverred)
            invalidate();
        return true;
    }

    bool control::on_mouse_exit()
    {
        _hovered = false;
        set_cursor(cursor::standard);
        if (_redraw_when_hoverred)
            invalidate();
        return true;
    }

    bool control::hovered() const noexcept
    {
        return _hovered;
    }

}