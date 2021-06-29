#ifndef VIEW_PUSH_BUTTON_H_
#define VIEW_PUSH_BUTTON_H_

#include <functional>
#include "control.h"

namespace View {

    class push_button : public control {
        public:
            using callback = std::function<void()>;

            push_button(
                float width, float height,
                size_constraint width_constraint = free_size,
                size_constraint hieght_constraint = free_size);
            ~push_button() override = default;

            void set_callback(callback c);

            bool on_mouse_button_up(const mouse_button button, float x, float y) override;
            bool on_mouse_button_down(const mouse_button button, float x, float y) override;
            bool on_mouse_exit() override;
            bool on_mouse_drag_cancel() override;

            void draw(NVGcontext*) override;

            void apply_color_theme(const color_theme& theme) override;

        private:
            void _update_pushed(bool);

            bool _pushed{false};
            callback _callback{[](){}};

            NVGcolor _hovered_border;
            NVGcolor _surface;
    };

}

#endif