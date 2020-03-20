#ifndef VIEW_CHECKBOX_H_
#define VIEW_CHECKBOX_H_

#include <functional>
#include "control.h"

namespace View {

    class checkbox : public control {

    public:
        using callback = std::function<void(bool checked)>;

        checkbox(float size, bool checked = false);
        ~checkbox() override = default;

        void set_callback(callback c) { _callback = c; }
        bool is_checked() const noexcept { return _checked; }

        bool on_mouse_button_up(mouse_button button, float x, float y) override;
        void draw(cairo_t*) override;

        void apply_color_theme(const color_theme& theme) override;
    private:
        void _switch_state();

        //  state
        bool _checked;
        callback _callback{[](bool){}};

        //  colors
        color _background;
        color _border;
        color _hovered_color;
        color _check_color;
    };

}

#endif