#ifndef VIEW_WIDGET_PROXY_H_
#define VIEW_WIDGET_PROXY_H_

#include <memory>

#include "widget.h"
#include "../display/common/display_controler.h"

namespace View {

    template <typename TChildren = widget>
    class widget_proxy : public widget, private display_controler
    {

    public:
        widget_proxy(float width, float height) noexcept
        : widget{width, height}
        {}

        widget_proxy(float width, float height, size_constraint width_constraint, size_constraint height_constraint)
        : widget{width, height, width_constraint, height_constraint}
        {}

        ~widget_proxy() override = default;

        void set_widget(std::weak_ptr<TChildren> w)
        {
            _children = w;

            if (auto sptr = w.lock()) {
                sptr->resize(width(), height());
                display_controler::set_widget(*sptr);
            }

            invalidate();
        }

        void reset_widget()
        {
            _children = {};
            invalidate();
        }

        /* Widget interface */
        bool resize(float width, float height) override
        {
            if (widget::constraint_match_size(width, height)) {
                if (auto sptr = _children.lock()) {
                    if (sptr->constraint_match_size(width, height)) {
                        widget::resize(width, height);
                        sptr->resize(width, height);
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    widget::resize(width, height);
                    return true;
                }
            }
            else {
                return false;
            }
        }

        //  Events
        bool on_key_up(const keycode key) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_key_up(key);
            else
                return false;
        }

        bool on_key_down(const keycode key) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_key_down(key);
            else
                return false;
        }

        bool on_text_input(std::string_view str) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_text_input(str);
            else
                return false;
        }

        bool on_mouse_enter() override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_enter();
            else
                return false;
        }

        bool on_mouse_exit() override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_exit();
            else
                return false;
        }

		bool on_mouse_move(float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_move(x, y);
            else
                return false;
        }

        bool on_mouse_wheel(float distance)  override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_wheel(distance);
            else
                return false;
        }


		bool on_mouse_button_down(const mouse_button button, float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_button_down(button, x, y);
            else
                return false;
        }

		bool on_mouse_button_up(const mouse_button button, float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_button_up(button, x, y);
            else
                return false;
        }

		bool on_mouse_dbl_click(float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_dbl_click(x, y);
            else
                return false;
        }

		bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_drag(button, x, y, dx, dy);
            else
                return false;
        }

		bool on_mouse_drag_start(const mouse_button button, float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_drag_start(button, x, y);
            else
                return false;
        }

		bool on_mouse_drag_end(const mouse_button button, float x, float y) override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_drag_end(button, x, y);
            else
                return false;
        }

        bool on_mouse_drag_cancel() override
        {
            if (auto sptr = _children.lock())
                return sptr->on_mouse_drag_cancel();
            else
                return false;
        }

        void draw(NVGcontext *vg) override
        {
            if (auto sptr = _children.lock())
                sptr->draw(vg);
        }

        void draw_rect(NVGcontext* vg, const rectangle<>& rect) override
        {
            if (auto sptr = _children.lock())
                sptr->draw_rect(vg, rect);

        }

        //  Color theme handling
        void apply_color_theme(const color_theme& theme) override
        {
            if (auto sptr = _children.lock())
                sptr->apply_color_theme(theme);
        }

    private:
        /* Dpy ctl interface */
        void invalidate_rect(const rectangle<>& rect) override
        {
            auto dpy_ctl = display_ctl();

            if (dpy_ctl != nullptr)
                dpy_ctl->invalidate_rect(rect);
        }

         void set_cursor(cursor c) override
         {
            auto dpy_ctl = display_ctl();

            if (dpy_ctl != nullptr)
                dpy_ctl->set_cursor(c);
        }

        std::weak_ptr<TChildren> _children{};
    };

}

#endif /* VIEW_WIDGET_PROXY_H_ */