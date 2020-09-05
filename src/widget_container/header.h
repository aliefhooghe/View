#ifndef VIEW_HEADER_H_
#define VIEW_HEADER_H_

#include "widget_wrapper_base.h"

namespace View {

    class header : public widget_wrapper_base<header> {
    public:
        header(
            std::unique_ptr<widget>&& root,
            float header_size = 14, float border_size = 4);

        ~header() override = default;

        bool resize(float width, float height) override;
        void draw(NVGcontext *vg) override;
        void draw_rect(NVGcontext *vg, const rectangle<>&) override;

        void apply_color_theme(const color_theme& theme) override;
    private:
        float _header_size;
        float _border_size;
        NVGcolor _background_color;
        NVGcolor _header_color;
    };

}

#endif
