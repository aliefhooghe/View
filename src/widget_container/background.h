#ifndef VIEW_BACKGROUND_H_
#define VIEW_BACKGROUND_H_

#include "widget_wrapper_base.h"

namespace View {

    class background : public widget_wrapper_base<background> {
    public:
        background(std::unique_ptr<widget>&& root);
        ~background() override = default;

        bool resize(float width, float height) override;
        void draw(NVGcontext *) override;
        void draw_rect(NVGcontext *, const rectangle<>& area) override;

        void apply_color_theme(const color_theme &theme) override;
    private:
        NVGcolor _background_color;
    };
}

#endif