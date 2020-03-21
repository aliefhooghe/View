#ifndef VIEW_TEXT_PUSH_BUTTON_H
#define VIEW_TEXT_PUSH_BUTTON_H

#include "push_button.h"

namespace View {

    class text_push_button : public push_button {

    public:
        template <typename TText>
        text_push_button(TText&& text, float width = 4.f, float height = 1.5f)
        : push_button{width, height}, _text{std::forward<TText>(text)}
        {}

        void draw(cairo_t *cr) override;
        void apply_color_theme(const color_theme& theme) override;

    private:
        std::string _text;
        color _text_color;
    };

}

#endif //VIEW_TEXT_PUSH_BUTTON_H
