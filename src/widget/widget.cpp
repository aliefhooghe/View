
#include "widget.h"
#include "display/common/display_controler.h"

namespace View {

    widget::widget(float width, float height) noexcept
    : _width{width}, _height{height},
        _width_constraint{free_size},
        _height_constraint{free_size}
    {}

    widget::widget(float width, float height, size_constraint width_constraint, size_constraint height_constraint) noexcept
    :   _width{width}, _height{height},
        _width_constraint{width_constraint},
        _height_constraint{height_constraint}
    {}

    void widget::freeze_size()
    {
        _width_constraint = size_constraint{width(), width()};
        _height_constraint = size_constraint{height(), height()};
    }

    bool widget::resize(float width, float height)
    {
        if (_width_constraint.contains(width) && _height_constraint.contains(height)) {
            _width = width;
            _height = height;
            return true;
        }
        else {
             std::cout << "resize_all failed" << std::endl;
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

    void widget::set_cursor(cursor c)
    {
        if (_display_ctl)
            _display_ctl->set_cursor(c);
    }

}