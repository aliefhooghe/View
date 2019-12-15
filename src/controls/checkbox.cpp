
#include "checkbox.h"
#include "drawing/cairo_helper.h"
#include "drawing/named_colors.h"

namespace View {

    checkbox::checkbox(float size, bool checked)
    : control{size, size}, _checked{checked}
    {
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
        /**
         *  \todo Prise en charge des thèmes : Probablement seulement les couleurs
         */

        const auto unit = width() / 8.f;

        rounded_rectangle(cr, 0, 0, width(), height(), unit);

        //  Draw background
        set_source(cr, 0x1E1E1EFF);
        cairo_fill_preserve(cr);

        //  Draw border
        set_source(cr, hovered() ? 0x45A1FFFF : named_colors::gray);
        cairo_set_line_width(cr, width() / 10.f);
        cairo_stroke(cr);

        //  Draw check
        if (_checked) {
            cairo_move_to(cr, 2.f * unit, 5.f * unit);
            cairo_line_to(cr, 3.f * unit, 6.f * unit);
            cairo_line_to(cr, 6.f * unit, 3.f * unit);

            set_source(cr, 0x45A1FFFF);
            cairo_set_line_width(cr, width() / 6.f);
            cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
            cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
            cairo_stroke(cr);
        }
    }

    void checkbox::_switch_state()
    {
        _checked = !_checked;
        _callback(_checked);
        invalidate();
    }

}