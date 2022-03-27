#ifndef VIEW_TEXT_INPUT_H_
#define VIEW_TEXT_INPUT_H_

#include <functional>
#include "control.h"

namespace View {

    /**
     *  \brief a simple text input widget
     */
    class text_input : public control {
    public:
        static constexpr float default_width = 140.f;
        static constexpr float default_height = 21.f;
        using callback = std::function<void()>;

        text_input(
            float width = default_width,
            float height = default_height);
        text_input(
            float width, float height,
            size_constraint width_constraint,
            size_constraint height_constraint);

        bool on_char_input(char) override;

        void draw(NVGcontext *vg) override;
        void apply_color_theme(const color_theme&) override;

        void set_text(const std::string& txt);
        void clear_text();
        const auto& get_text() const noexcept { return _text; }
        void set_enter_callback(callback enter_callback);
    private:
        NVGcolor _surface_color;
        NVGcolor _text_color;
        NVGcolor _hoverred_border_color;
        std::string _text{""};
        callback _enter_callback{[](){}};
    };

}

#endif