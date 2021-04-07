#include <cmath>
#include "map_wrapper.h"

namespace View {

    map_wrapper_widget_holder::map_wrapper_widget_holder(
            map_wrapper& parent, float x, float y,
            std::unique_ptr<widget>&& children)
    :   widget_holder<>{parent, 0.f, 0.f, std::move(children)},
        _parent{&parent}
    {
        (void)x, (void)y;
    }

    void map_wrapper_widget_holder::invalidate_rect(const rectangle<>& rect)
    {
        /**
         *
         * Problème :
         *  - L'enfant vas demander de redessiner sa zone uniquement, qui peut carrement ne plus être visible
         *
         * Quick fix :
         *  - Redessiner toute la map
         *
         * A faire :
         *  - Compliqué car
         *
         * Solution ?
         *      Prevoir une zone d'edition enorme ?
         *      Comment gerer correctmenent le redimensionement
         * Bref : rework la map
         *
         */
        /** @todo optimize : Redraw rect if it overlap view ! **/
        // std::cout << "map_wrapper_widget_holder::invalidate_rect : map origin = " << _parent->_origin_x << ", " << _parent->_origin_y << std::endl;
        // std::cout << "map_wrapper_widget_holder::invalidate_rect : " << rect << " => " << rect.translate(_pos_x - _parent->_origin_x, _pos_y - _parent->_origin_y) << std::endl;
        // _parent->invalidate_rect(rect.translate(_pos_x - _parent->_origin_x, _pos_y - _parent->_origin_y));

        _parent->invalidate();
    }

    void map_wrapper_widget_holder::invalidate_widget()
    {
        //  Everything in content must be redrawn : Invalidate the map wrapper
        _parent->invalidate();
    }

    /**
     *  map wrapper implementation
     **/

    map_wrapper::map_wrapper(
        std::unique_ptr<widget>&& root,
        float width, float height)
    :   widget_wrapper_base{std::move(root), width, height}
    {}

    map_wrapper::map_wrapper(
        std::unique_ptr<widget>&& root,
        float width, float height,
        size_constraint width_constrain, size_constraint height_constrain)
    :   widget_wrapper_base{std::move(root), width, height, width_constrain, height_constrain}
    {}

    void map_wrapper::reset_view() noexcept
    {
        _set_origin(0.f, 0.f);
        _scale = 1.f;
        invalidate();
    }

	bool map_wrapper::on_mouse_move(float x, float y)
    {
        return widget_wrapper_base::on_mouse_move(_x_to_content(x), _y_to_content(y));
    }

	bool map_wrapper::on_mouse_button_down(const mouse_button button, float x, float y)
    {
        return widget_wrapper_base::on_mouse_button_down(button, _x_to_content(x), _y_to_content(y));
    }

	bool map_wrapper::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        return widget_wrapper_base::on_mouse_button_up(button, _x_to_content(x), _y_to_content(y));
    }

	bool map_wrapper::on_mouse_dbl_click(float x, float y)
    {
        return widget_wrapper_base::on_mouse_dbl_click(_x_to_content(x), _y_to_content(y));
    }

	bool map_wrapper::on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy)
    {
        if (!widget_wrapper_base::on_mouse_drag(button, _x_to_content(x), _y_to_content(y), dx / _scale, dy / _scale)) {
            _translate_origin(-dx, -dy);
            invalidate();
        }

        return true;
    }

	bool map_wrapper::on_mouse_drag_start(const mouse_button button, float x, float y)
    {
        return widget_wrapper_base::on_mouse_drag_start(button, _x_to_content(x), _y_to_content(y));
    }

	bool map_wrapper::on_mouse_drag_end(const mouse_button button, float x, float y)
    {
        return widget_wrapper_base::on_mouse_drag_end(button, _x_to_content(x), _y_to_content(y));
    }

    void map_wrapper::draw(NVGcontext *vg)
    {
        //  Clip the drawing area
        nvgIntersectScissor(vg, 0, 0, widget_wrapper_base::width(), widget_wrapper_base::height());

        //  Translate and draw
        nvgSave(vg);
        nvgTranslate(vg, -_origin_x, -_origin_y);
        nvgScale(vg, _scale, _scale);
        widget_wrapper_base::draw(vg);
        nvgRestore(vg);
    }

    bool map_wrapper::on_mouse_wheel(float x, float y, float distance)
    {
        if (!widget_wrapper_base::on_mouse_wheel(_x_to_content(x), _y_to_content(y), distance)) {
            const auto factor = std::pow(1.0625f, distance);

            _scale *= factor;
            _origin_x = factor * (_origin_x + x) - x;
            _origin_y = factor * (_origin_y + y) - y;

            invalidate();
        }

        return true;
    }

    void map_wrapper::_translate_origin(float dx, float dy) noexcept
    {
        _origin_x += dx;
        _origin_y += dy;
    }

    void map_wrapper::_set_origin(float x, float y) noexcept
    {
        _origin_x = x;
        _origin_y = y;
    }

}