

#include "widget_adapter.h"

namespace View {

    widget_adapter::widget_adapter(widget& root, float pixel_per_unit)
    :   display_controler{root},
        _root{root}, _pixel_per_unit{pixel_per_unit}
    {
        _display_width = static_cast<unsigned int>(pixel_per_unit * _root.width());
        _display_height = static_cast<unsigned int>(pixel_per_unit * _root.height());
    }

    void widget_adapter::resize_display(unsigned int width, unsigned int height)
    {
        _display_width = width;
        _display_height = height;

        float widget_width, widget_height;
        _coord_display2widget(width, height, widget_width, widget_height);

        _root.resize(widget_width, widget_height);
    }

    void widget_adapter::sys_draw(cairo_t *cr)
    {
        begin_drawing(cr);

        cairo_scale(
            cr,
            _pixel_per_unit,
            _pixel_per_unit);
        _root.draw(cr);

        finish_drawing(cr);
    }

    void widget_adapter::sys_draw_rect(cairo_t *cr, unsigned int top, unsigned int bottom, unsigned int left, unsigned int right)
    {
        begin_drawing(cr);

        //  Scale to widget coordinate
        cairo_scale(
            cr,
            _pixel_per_unit,
            _pixel_per_unit);

        //  Compute redraw rect in widget coordinate
        const auto redraw_rect =
            make_rectangle(
                static_cast<float>(top) / _pixel_per_unit,
                static_cast<float>(bottom) / _pixel_per_unit,
                static_cast<float>(left) / _pixel_per_unit,
                static_cast<float>(right) / _pixel_per_unit);

        _root.draw_rect(cr, redraw_rect);

        finish_drawing(cr);
    }

	bool widget_adapter::sys_mouse_move(unsigned int cx, unsigned int cy)
    {
        bool ret = false;
        float old_cursor_x = _cursor_fx;
        float old_cursor_y = _cursor_fy;

        _coord_display2widget(cx, cy, _cursor_fx, _cursor_fy);

        if (!_is_draging && _pressed_button_count > 0) {
            _is_draging = true;
            ret = _root.on_mouse_drag_start(_draging_button, old_cursor_x, old_cursor_y);
        }

        if (_is_draging) {
            return _root.on_mouse_drag(
                _draging_button,
                _cursor_fx, _cursor_fy,
                _cursor_fx - old_cursor_x,
                _cursor_fy - old_cursor_y) || ret;
        }
        else {
            return _root.on_mouse_move(_cursor_fx, _cursor_fy) || ret;
        }
    }

	bool widget_adapter::sys_mouse_enter(void)
    {
        return _root.on_mouse_enter();
    }

	bool widget_adapter::sys_mouse_exit(void)
    {
        _pressed_button_count = 0u;
        _is_draging = false;
        return _root.on_mouse_exit();
    }

	bool widget_adapter::sys_mouse_button_down(const mouse_button button)
    {
        _pressed_button_count++;
        _draging_button = button;
        return _root.on_mouse_button_down(button, _cursor_fx, _cursor_fy);
    }

	bool widget_adapter::sys_mouse_button_up(const mouse_button button)
    {
        if (_pressed_button_count > 0)
            _pressed_button_count--;

        if (_is_draging && button == _draging_button) {
            _is_draging = false;
            return _root.on_mouse_drag_end(button, _cursor_fx, _cursor_fy) ||
                _root.on_mouse_button_up(button, _cursor_fx, _cursor_fy);
        }
        else {
            return _root.on_mouse_button_up(button, _cursor_fx, _cursor_fy);
        }
    }

	bool widget_adapter::sys_mouse_wheel(const float distance)
    {
        return _root.on_mouse_wheel(distance);
    }

	bool widget_adapter::sys_mouse_dbl_click(void)
    {
        return _root.on_mouse_dbl_click(_cursor_fx, _cursor_fy);
    }

	bool widget_adapter::sys_key_down(const keycode key)
    {
        return _root.on_key_down(key);
    }

	bool widget_adapter::sys_key_up(const keycode key)
    {
        return _root.on_key_up(key);
    }

    void widget_adapter::begin_drawing(cairo_t *cr)
    {
        cairo_save(cr);
        cairo_push_group(cr);
    }

    void widget_adapter::finish_drawing(cairo_t *cr)
    {
        cairo_pop_group_to_source(cr);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    void widget_adapter::invalidate_rect(const rectangle<>& rect)
    {
        draw_area area;
        _coord_widget2display(rect.left, rect.top, area.left, area.top);
        _coord_widget2display(rect.right, rect.bottom, area.right, area.bottom);
        sys_invalidate_rect(area);
    }

    float widget_adapter::widget_pos_x()
    {
        return 0.f;
    }

    float widget_adapter::widget_pos_y()
    {
        return 0.f;
    }

    void widget_adapter::_coord_display2widget(int x, int y, float &fx, float &fy)
    {
        fx = x / _pixel_per_unit;
        fy = y / _pixel_per_unit;
    }

    void widget_adapter::_coord_widget2display(float fx, float fy, int &x, int &y)
    {
        x = fx * _pixel_per_unit;
        y = fy * _pixel_per_unit;
    }
}