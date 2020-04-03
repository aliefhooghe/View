#ifndef VIEW_TEXT_INPUT_H_
#define VIEW_TEXT_INPUT_H_

#include "control.h"

namespace View {

    /**
     *  \brief a simple text input widget
     */
    class text_input : public control {
    public:
        text_input(float width = 10, float height = 1.5);

        bool on_text_input(std::string_view) override;

        void draw(cairo_t *cr) override;
        void apply_color_theme(const color_theme&) override;

        void clear_text();
        const auto& get_text() const noexcept { return _text; }
    private:
        View::color _background_color;
        View::color _text_color;
        View::color _hovered_border_color;
        std::string _text{""};
    };

}

#endif