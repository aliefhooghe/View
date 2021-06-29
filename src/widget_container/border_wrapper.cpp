
#include "border_wrapper.h"

namespace View {

    border_wrapper::border_wrapper(
            std::unique_ptr<widget>&& root,
            float border_top,
            float border_bottom,
            float border_left,
            float border_right)
    :   widget_wrapper_base<border_wrapper>{
            std::move(root), 0.f, 0.f, 1.f, 1.f},
        _border_top{border_top},
        _border_bottom{border_bottom},
        _border_left{border_left},
        _border_right{border_right}
    {
        _root.set_pos(_border_left, _border_top);

        const auto width_offset = _border_left + _border_right;
        const auto height_offset = _border_top + _border_bottom;

        widget_wrapper_base<border_wrapper>::resize(
            _root->width() + width_offset,
            _root->height() + height_offset);

        set_size_constraints(
            size_constraint{
                _root->width_constraint().min + width_offset,
                _root->width_constraint().max + width_offset
            },
            size_constraint{
                _root->height_constraint().min + height_offset,
                _root->height_constraint().max + height_offset
            });
    }

    bool border_wrapper::resize(float width, float height)
    {
        const auto width_offset = _border_left + _border_right;
        const auto height_offset = _border_top + _border_bottom;

        if (_root->resize(width - width_offset, height - height_offset)) {
            widget_wrapper_base<border_wrapper>::resize(width, height);
            return true;
        }
        else {
            return false;
        }
    }

}