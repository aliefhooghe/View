#ifndef VIEW_COLOR_PANEL_H_
#define VIEW_COLOR_PANEL_H_

#include "panel.h"
#include "drawing/color.h"
#include "drawing/cairo_helper.h"

namespace View {

    template <typename TChildren = widget>
    class color_panel : public panel<TChildren> {

    public:
        color_panel(float width, float height, color background = 0x2A2A2EFFu)
        :   panel<TChildren>{width, height},
            _background{background}
        {}

        color_panel(float width, float height, size_constraint width_constraint, size_constraint height_constraint, color background = 0x2A2A2EFFu)
        :   panel<TChildren>{width, height, width_constraint, height_constraint},
            _background{background}
        {}

        ~color_panel() override = default;

        using panel<TChildren>::insert_widget;
        using panel<TChildren>::remove_widget;
    protected:
        void draw_background(cairo_t *cr) override
        {
            cairo_rectangle(cr, 0, 0, panel<TChildren>::width(), panel<TChildren>::height());
            set_source(cr, _background);
            cairo_fill(cr);
        }

        color _background;
    };

}

#endif