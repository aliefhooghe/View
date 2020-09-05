
#include <cmath>

#include "header.h"
#include "drawing/shadowed.h"

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

    void header::draw(NVGcontext *vg)
    {
        const auto rect_radius = _header_size / 3;

        nvgBeginPath(vg);
        
        // Main Shape
        nvgRoundedRect(vg,
            _border_size, _border_size,
            _root.get()->width(), _root.get()->height() + _header_size,
            rect_radius);

        //  Draw Header
        nvgFillColor(vg, _header_color);
        nvgFill(vg);

        //  Draw Border
        nvgStrokeWidth(vg, 3);
        nvgStrokeColor(vg, _header_color);
        nvgStroke(vg);

        //  Draw background
        shadowed_down_rounded_rect(vg, 
            _border_size, _border_size + _header_size,
            _root.get()->width(), _root.get()->height(),
            rect_radius, _background_color);

        //  Clip to ensure that child only draw on background
        nvgIntersectScissor(vg, 
            _border_size, _border_size + _header_size,
            _root.get()->width(), _root.get()->height());

        //  Draw child
        widget_wrapper_base<header>::draw(vg);
    }

    void header::draw_rect(NVGcontext *vg, const rectangle<>& area)
    {
        auto root_area =
            make_rectangle(
                _border_size + _header_size,
                _border_size + _header_size + _root.get()->height(),
                _border_size, _border_size + _root.get()->height());

        if (root_area.contains(area))
        {
            //  Draw background on area (Header footer ???)
            nvgRect(vg, area.left, area.top, area.width(), area.height());
            nvgFillColor(vg, _background_color);
            nvgFill(vg);

            //  Draw content on area
            draw_widgets(vg, area);
        }
        else
        {
            //  Redraw Everything : TODO optimize me !!
            draw(vg);
        }
    }

    void header::apply_color_theme(const View::color_theme &theme)
    {
        widget_wrapper_base<header>::apply_color_theme(theme);
        _header_color = theme.primary;
        _background_color = theme.surface_dark;
    }

}