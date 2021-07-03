
#include <cmath>

#include "header.h"
#include "drawing/shadowed.h"

namespace View {

    header::header(
        std::unique_ptr<widget>&& root, color_theme::color background,
        float header_size, float border, float internal_border)
    :   _background_color_id{background},
        widget_wrapper_base<header>{std::move(root), 0.f, 0.f, 1.f, 1.f},
        _header_size{header_size}, _border{border}, _internal_border{internal_border}
    {
        const auto border_offset = 2.f * (_border + _internal_border);
        size_constraint height_constraint{};
        size_constraint width_constraint{};

        width_constraint.min = _root->width_constraint().min + border_offset;
        width_constraint.max = _root->width_constraint().max + border_offset;
        height_constraint.min = _root->height_constraint().min + border_offset + _header_size;
        height_constraint.max = _root->height_constraint().max + border_offset + _header_size;

        apply_color_theme(default_color_theme);

        widget_wrapper_base<header>::resize(
            _root->width() + border_offset,
            _root->height() + header_size + border_offset);
        set_size_constraints(width_constraint, height_constraint);
        _root.set_pos(_border + _internal_border,  _header_size + _border + _internal_border);
    }

    bool header::resize(float w, float h)
    {
        const auto border_offset = 2.f * (_border + _internal_border);

        if (width_constraint().contains(w) &&
            height_constraint().contains(h))
        {
            if (_root->resize(
                w - border_offset,
                h - (_header_size + border_offset)))
            {
                widget_wrapper_base<header>::resize(w, h);
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
            0.f, 0.f,
            width(), height(),
            rect_radius);

        //  Draw Header
        nvgFillColor(vg, _header_color);
        nvgFill(vg);

        //  Draw background
        shadowed_down_rounded_rect(vg,
            _border, _header_size + _border,
            _root->width() + 2.f * _internal_border,
            _root->height() + 2.f * _internal_border,
            rect_radius, _background_color);

        //  Clip to ensure that child only draw on background
        nvgIntersectScissor(vg,
            _border, _header_size + _border,
            _root->width() + 2.f * _internal_border,
            _root->height() + 2.f * _internal_border);

        //  Draw child
        widget_wrapper_base<header>::draw(vg);
    }

    void header::draw_rect(NVGcontext *vg, const rectangle<>& area)
    {
        auto root_area =
            make_rectangle(
                _header_size + _border, _header_size + _border + 2.f * _internal_border + _root->height(),
                _border, _border + 2.f * _internal_border + _root->width());

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
        _background_color = theme.get(_background_color_id);
    }

}