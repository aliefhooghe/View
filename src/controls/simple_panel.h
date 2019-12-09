#ifndef VIEW_SIMPLE_PANEL_H_
#define VIEW_SIMPLE_PANEL_H_

#include "widget/widget_container.h"
#include "drawing/color.h"
#include "drawing/cairo_helper.h"

#include <iostream>

namespace View {

    template <typename Children = widget>
    class simple_panel : public widget_container<Children> {

    public:
        simple_panel(float width, float height, color c = 0x00FF00FFu)
        :   widget_container<Children>{width, height}, _background{c}
        {}

        virtual ~simple_panel() = default;
    protected:
        void draw_background(cairo_t *cr) override
        {
            set_source(cr, _background);
            cairo_rectangle(cr, 0, 0, widget_container<Children>::width(),  widget_container<Children>::height());
            cairo_fill(cr);
        }

        color _background;
    };
}

#endif