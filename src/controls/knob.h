#ifndef VIEW_KNOB_H_
#define VIEW_KNOB_H_

#include <functional>
#include "control.h"

namespace View {

    class knob : public control {

    public:
        using callback = std::function<void(float)>;

        knob(float size = 56.f, float initial_value = 0.5f, bool display_value = true);
        ~knob() override = default;

        void set_callback(callback);

        void set_value(float);
        float get_value() const noexcept { return _value; }

        bool on_mouse_drag(mouse_button, float, float, float, float) override;
        bool on_mouse_wheel(float) override;
        void draw(NVGcontext*) override;

        void apply_color_theme(const color_theme& theme) override;

    private:
        callback _callback{[](float){}};
        float _value;
        bool _display_value;

        NVGcolor _background;
        NVGcolor _surface;
        NVGcolor _hovered_border;
        NVGcolor _track;
        NVGcolor _text;
    };

}

#endif