#ifndef VIEW_LAYOUT_SEPARATOR_H_
#define VIEW_LAYOUT_SEPARATOR_H_

#include <functional>

#include "controls/control.h"
#include "widget/orientation.h"

namespace View {

    class layout_separator : public control {

    public:
        using callback = std::function<void(float)>;

        layout_separator(float width, float height, orientation orient);
        ~layout_separator() override = default;

        bool on_mouse_drag(mouse_button, float x, float y, float dx, float dy) override;
        void set_callback(callback c) { _callback = c; }

    private:
        orientation _orientation;
        callback _callback{[](float){}};
    };

}

#endif