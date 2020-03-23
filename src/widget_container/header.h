#ifndef VIEW_HEADER_H_
#define VIEW_HEADER_H_

#include "widget_wrapper_base.h"
#include "drawing/color.h"

namespace View {

    class header : public widget_wrapper_base<header> {
    public:
        header(
            std::unique_ptr<widget>&& root,
            float header_size = 1.2f, float border_size = 0.3f);

        ~header() override = default;

        bool resize(float width, float height) override;
        void draw(cairo_t *cr) override;
        void draw_rect(cairo_t* cr, const rectangle<>&) override;

        void apply_color_theme(const color_theme& theme) override;
    private:
        float _header_size;
        float _border_size;
        color _background_color;
        color _header_color;
    };

}

#endif
