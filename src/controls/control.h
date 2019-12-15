#ifndef VIEW_CONTROL_H_
#define VIEW_CONTROL_H_

#include "widget/widget.h"

namespace View {

    class control : public widget {

    public:
        control(float width, float height);

        bool on_mouse_enter() override;
        bool on_mouse_exit() override;

    protected:
        bool hovered() const noexcept;

    private:
        bool _hovered{false};
        //  Todo enabled ?
    };

}

#endif