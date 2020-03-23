#ifndef VIEW_BACKGROUND_H_
#define VIEW_BACKGROUND_H_

#include "widget_wrapper_base.h"
#include "drawing/color.h"

namespace View {

    class background : public widget_wrapper_base<background> {
    public:
        background(std::unique_ptr<widget>&& root);
        ~background() override = default;

        bool resize(float width, float height) override;
        void draw(cairo_t *cr) override;
        void draw_rect(cairo_t* cr, const rectangle<>& area) override;

        void apply_color_theme(const color_theme &theme) override;
    private:
        color _background_color;
    };
}

#endif