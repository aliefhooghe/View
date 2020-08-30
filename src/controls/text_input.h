#ifndef VIEW_TEXT_INPUT_H_
#define VIEW_TEXT_INPUT_H_

#include "control.h"

namespace View {

    /**
     *  \brief a simple text input widget
     */
    class text_input : public control {
    public:
        text_input(float width = 140, float height = 21);

        bool on_text_input(std::string_view) override;

        void draw(NVGcontext *vg) override;
        void apply_color_theme(const color_theme&) override;

        void clear_text();
        const auto& get_text() const noexcept { return _text; }
    private:
        NVGcolor _background_color;
        NVGcolor _surface_color;
        NVGcolor _text_color;
        NVGcolor _hoverred_border_color;
        std::string _text{""};
    };

}

#endif