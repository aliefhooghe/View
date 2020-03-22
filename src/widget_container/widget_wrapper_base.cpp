#include "widget_wrapper_base.h"

namespace View {

    widget_wrapper_base::widget_wrapper_base(std::unique_ptr<widget>&& root)
    : widget_container<widget_wrapper_base>{root->width(), root->height(), root->width_constraint(), root->height_constraint()},
        _root{*this, 0, 0, std::move(root)}
    {
    }

    widget_wrapper_base::widget_wrapper_base(std::unique_ptr<widget>&& root, float width, float height)
    : widget_container<widget_wrapper_base>{width, height},
        _root{*this, 0, 0, std::move(root)}
    {
    }

    widget_wrapper_base::widget_wrapper_base(
        std::unique_ptr<widget>&& root,
        float width, float height,
        size_constraint width_constrain, size_constraint height_constrain)
    :   widget_container<widget_wrapper_base>{width, height, width_constrain, height_constrain},
        _root{*this, 0, 0, std::move(root)}
    {
    }

    void widget_wrapper_base::draw(cairo_t *cr)
    {
        widget_container<widget_wrapper_base>::draw_widgets(cr);
    }

    void widget_wrapper_base::draw_rect(cairo_t* cr, const rectangle<>& area)
    {
        widget_container<widget_wrapper_base>::draw_widgets(cr, area);
    }

}