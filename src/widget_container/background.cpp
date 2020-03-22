
#include "background.h"
#include "drawing/cairo_helper.h"

namespace View {

    background::background(std::unique_ptr<widget>&& root)
    :   widget_wrapper_base{std::move(root)}
    {
        apply_color_theme(default_color_theme);
    }

    bool background::resize(float width, float height)
    {
        if (width_constraint().contains(width) &&
            height_constraint().contains(height) &&
            _root->resize(width, height))
        {
            widget_wrapper_base::resize(width, height);
            return true;
        }
        else {
            return false;
        }
    }

    void background::draw(cairo_t *cr)
    {
        //  Draw background
        cairo_rectangle(cr, 0, 0, width(), height());
        set_source(cr, _background_color);
        cairo_fill(cr);

        //  Draw widgets
        widget_wrapper_base::draw(cr);
    }

    void background::draw_rect(cairo_t* cr, const rectangle<>& area)
    {
        //  Draw background on area (Header footer ???)
        cairo_rectangle(cr, area.left, area.top, area.width(), area.height());
        set_source(cr, _background_color);
        cairo_fill(cr);

        //  Draw widgets
        widget_wrapper_base::draw_rect(cr, area);
    }

    void background::apply_color_theme(const View::color_theme &theme)
    {
        //  apply on childrens
        widget_wrapper_base::apply_color_theme(theme);

        //  set background color
        _background_color = theme.background;
    }
}