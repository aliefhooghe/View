
#include "widget.h"
#include "display/common/display_controler.h"

namespace View {

    widget::widget(float width, float height) noexcept
    : _width{width}, _height{height}
    {
    }

    bool widget::resize(float width, float height)
    {
        if (width > 0.0f && height > 0.0f) {
            _width = width;
            _height = height;
            return true;
        }
        else {
            return false;
        }
    }

    bool widget::contains(float x, float y)
    {
        return x >= 0.0f && x <= _width && y >= 0.0f && y <= _height;
    }

    void widget::invalidate()
    {
        if (_display_ctl)
            _display_ctl->invalidate_widget();
    }

    void widget::invalidate_rect(const rectangle<>& rect)
    {
        if (_display_ctl)
            _display_ctl->invalidate_rect(rect);
    }

}