#ifndef VIEW_PUSH_BUTTON_H_
#define VIEW_PUSH_BUTTON_H_

#include <functional>
#include "control.h"

namespace View {

    class push_button : public control {
        public:
            using callback = std::function<void(push_button&)>;

            push_button(float width, float height);
            ~push_button() override = default;

            void set_callback(callback c);

            bool on_mouse_button_up(const mouse_button button, float x, float y) override;
            bool on_mouse_button_down(const mouse_button button, float x, float y) override;
            bool on_mouse_drag_end(const mouse_button button, float x, float y) override;
            bool on_mouse_drag_cancel() override;

            void draw(cairo_t*) override;

        private:
            void _update_pushed(bool);

            bool _pushed{false};
            callback _callback{[](push_button&){}};
    };

}

#endif