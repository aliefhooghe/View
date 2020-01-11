
#include "checkbox.h"
#include "drawing/cairo_helper.h"
#include "drawing/named_colors.h"

namespace View {

    checkbox::checkbox(float size, bool checked)
    : control{size, size}, _checked{checked}
    {
        apply_color_theme(default_color_theme);
    }

    bool checkbox::on_mouse_button_up(mouse_button button, float x, float y)
    {
        _switch_state();
        return true;
    }

    bool checkbox::on_mouse_drag_end(const mouse_button button, float x, float y)
    {
        if (contains(x, y))
            _switch_state();
        return true;
    }

    void checkbox::draw(cairo_t *cr)
    {
        const auto unit = width() / 8.f;

        rounded_rectangle(cr, 0, 0, width(), height(), unit);

        if (_checked) {
            //  Draw background
            set_source(cr, _background);
            cairo_fill(cr);

            //  Draw check
            cairo_move_to(cr, 2.f * unit, 5.f * unit);
            cairo_line_to(cr, 3.f * unit, 6.f * unit);
            cairo_line_to(cr, 6.f * unit, 3.f * unit);

            set_source(cr, _check_color);
            cairo_set_line_width(cr, width() / 6.f);
            cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
            cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
            cairo_stroke(cr);
        }
        else {
            //  Draw border
            set_source(cr, hovered() ? _hovered_color : _border);
            cairo_set_line_width(cr, width() / 10.f);
            cairo_stroke(cr);
        }

    }

    void checkbox::apply_color_theme(const View::color_theme& theme)
    {
        _background = theme.secondary_dark;
        _border = theme.on_surface;
        _hovered_color = theme.secondary;
        _check_color = theme.on_secondary;
    }

    void checkbox::_switch_state()
    {
        _checked = !_checked;
        _callback(_checked);
        invalidate();
    }

}