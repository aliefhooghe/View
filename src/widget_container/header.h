#ifndef VIEW_HEADER_H_
#define VIEW_HEADER_H_

#include "widget_wrapper_base.h"

namespace View {

    class header : public widget_wrapper_base<header> {
    public:
        header(
            std::unique_ptr<widget>&& root, color_theme::color background,
            float header_size, float border, float internal_border);

        ~header() override = default;

        bool resize(float width, float height) override;
        void draw(NVGcontext *vg) override;
        void draw_rect(NVGcontext *vg, const rectangle<>&) override;

        void apply_color_theme(const color_theme& theme) override;
    private:
        const float _header_size;
        const float _border;
        const float _internal_border;
        const color_theme::color _background_color_id;
        NVGcolor _background_color;
        NVGcolor _header_color;
    };

}

#endif
