#ifndef VIEW_CONTROL_H_
#define VIEW_CONTROL_H_

#include "widget/widget.h"

namespace View {

    class control : public widget {

    public:
        control(float width, float height, cursor c = cursor::hand, bool redraw_when_hoverred = true);
        control(
            float width, float height,
            size_constraint width_constraint, size_constraint height_constraint,
            cursor c = cursor::hand, bool redraw_when_hoverred = true);

        ~control() override = default;

        bool on_mouse_enter() override;
        bool on_mouse_exit() override;
        bool hovered() const noexcept;
        void set_hover_cursor(cursor c) noexcept;

    private:
        bool _hovered{false};
        cursor _cursor;
        bool _redraw_when_hoverred;
        //  Todo enabled ?
    };

}

#endif