
#include <cmath>

#include "header.h"
#include "drawing/cairo_helper.h"

namespace View {

    header::header(
        std::unique_ptr<widget>&& root,
        float header_size, float border_size)
    :   widget_wrapper_base<header>{
            std::move(root),
            root->width() + 2.f * border_size,
            root->height() + header_size + 2.f * border_size,
            size_constraint{
                root->width_constraint().min + 2.f * (border_size + header_size),   //  avoid overlaping of header arcs
                root->width_constraint().max + 2.f * border_size},
            size_constraint{
                root->height_constraint().min + 2.f * (border_size + header_size),
                root->height_constraint().max + header_size + 2.f * border_size}},
        _header_size{header_size},
        _border_size{border_size}
    {
        apply_color_theme(default_color_theme);
        _root.set_pos(_border_size, _border_size + _header_size);
    }

    bool header::resize(float width, float height)
    {
        if (width_constraint().contains(width) &&
            height_constraint().contains(height))
        {
            if (_root.get()->resize(
                width - 2.f * _border_size,height - (_header_size + 2.f * _border_size)))
            {
                widget_wrapper_base<header>::resize(width, height);
                return true;
            }
        }

        return false;

    }

    void header::draw(cairo_t *cr)
    {
        // Main Shape
        rounded_rectangle(cr,
            _border_size, _border_size,
            _root.get()->width(), _root.get()->height() + _header_size,
            _header_size / 2.5f);

        //  Draw Header
        set_source(cr, _header_color);
        cairo_fill_preserve(cr);

        //  Draw Border
        set_source(cr, _header_color);
        cairo_set_line_width(cr, 0.3f);
        cairo_stroke_preserve(cr);

        //  Ensure _root don't draw outsied
        cairo_clip(cr);

        //  Clip to not draw background on header
        cairo_rectangle(
            cr,
            _border_size, _border_size + _header_size,
            _root.get()->width(), _root.get()->height());
        cairo_clip_preserve(cr);

        //  Draw background
        set_source(cr, _background_color);
        cairo_fill(cr);

        //  Draw content
        widget_wrapper_base<header>::draw(cr);
    }

    void header::draw_rect(cairo_t* cr, const rectangle<>& area)
    {
        auto root_area =
            make_rectangle(
                _border_size + _header_size,
                _border_size + _header_size + _root.get()->height(),
                _border_size, _border_size + _root.get()->height());

        if (root_area.contains(area))
        {
            //  Draw background on area (Header footer ???)
            cairo_rectangle(cr, area.left, area.top, area.width(), area.height());
            set_source(cr, _background_color);
            cairo_fill(cr);

            //  Draw content on area
            draw_widgets(cr, area);
        }
        else
        {
            //  Redraw Everything : TODO optimize me !!
            draw(cr);
        }
    }

    void header::apply_color_theme(const View::color_theme &theme)
    {
        widget_wrapper_base<header>::apply_color_theme(theme);
        _header_color = theme.primary;
        _background_color = theme.surface;
    }

}