#ifndef VIEW_WIDGET_CONTAINER_H_
#define VIEW_WIDGET_CONTAINER_H_

#include <memory>

#include "widget/widget.h"
#include "display/common/display_controler.h"

namespace View {

    template <typename TDerived, typename TChildren>
    class widget_container : public widget {

    protected:
        class widget_holder : private display_controler {

        public:
            widget_holder(
                widget_container& parent,
                float x, float y, std::unique_ptr<TChildren>&& w)
            :   _parent{parent}, display_controler{*w},
                _pos_x{x}, _pos_y{y}, _widget_instance{std::move(w)}
            {}

            widget_holder(const widget_holder& x) noexcept = delete;
            widget_holder(widget_holder&& other) noexcept = default;

            auto pos_x() const noexcept { return _pos_x; }
            auto pos_y() const noexcept { return _pos_y; }

            auto get() noexcept
            {
                return _widget_instance.get();
            }

        private:
            /**
             *      Display controler interface
             */

            void invalidate_widget() override
            {
                _parent.invalidate_rect(
                    make_rectangle(
                        _pos_y, _pos_y + _widget_instance->height(),
                        _pos_x, _pos_x + _widget_instance->width()));
            }

            void invalidate_rect(const rectangle<>& rect) override
            {
                _parent.invalidate_rect(rect.translate(_pos_x, _pos_y));
            }

            void set_cursor(cursor c) override
            {
                _parent.set_cursor(c);
            }

            float _pos_x;
            float _pos_y;
            std::unique_ptr<TChildren> _widget_instance;
            widget_container& _parent;
        };

    public:
        widget_container(float width, float height)
        : widget{width, height}
        {}
        ~widget_container() override = default;


        //  Events
        bool on_key_up(const keycode key) override;
        bool on_key_down(const keycode key) override;
        bool on_mouse_enter() override;
		bool on_mouse_exit() override;
		bool on_mouse_move(float x, float y) override;
		bool on_mouse_wheel(float distance) override;
		bool on_mouse_button_down(const mouse_button button, float x, float y) override;
		bool on_mouse_button_up(const mouse_button button, float x, float y) override;
		bool on_mouse_dbl_click(float x, float y) override;
		bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy) override;
		bool on_mouse_drag_start(const mouse_button button, float x, float y) override;
		bool on_mouse_drag_end(const mouse_button button, float x, float y) override;
        bool on_mouse_drag_cancel() override;

    protected:
        void draw_widgets(cairo_t *cr);
        void draw_widgets(cairo_t *cr, const rectangle<>& rect);

    private:
        widget_holder *widget_at(float x, float y)
        {
            return static_cast<TDerived*>(this)->widget_at(x, y);
        }

        template <typename TFunction>
        void foreach_holder(TFunction func)
        {
            static_cast<TDerived*>(this)->foreach_holder(func);
        }

        widget_holder* _focused_widget{ nullptr };
        bool _draging{false};
    };

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_key_up(const keycode key)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_key_up(key);
        else
            return false;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_key_down(const keycode key)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_key_up(key);
        else
            return false;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_enter()
    {
        return false;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_exit()
    {
        if (_focused_widget) {
            if (_draging)
                _focused_widget->get()->on_mouse_drag_cancel();
            _focused_widget->get()->on_mouse_exit();
            _focused_widget = nullptr;
        }

        return true;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_move(float x, float y)
    {
        if (_focused_widget) {
            const auto x_rel = x - _focused_widget->pos_x();
            const auto y_rel = y - _focused_widget->pos_y();

            //  May avoid uing widget_at too much
            if (_focused_widget->get()->contains(x_rel, y_rel)) {
                return _focused_widget->get()->on_mouse_move(x_rel, y_rel);
            }
            else {
                auto *child = widget_at(x, y);
                bool used_event =
                    _focused_widget->get()->on_mouse_exit();

                if (child)
                    used_event |= child->get()->on_mouse_enter();

                _focused_widget = child;

                return used_event;
            }
        }
        else {
            if (auto *child = widget_at(x, y)) {
                _focused_widget = child;
                return child->get()->on_mouse_enter();
            }
            else {
                return false;
            }
        }
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_wheel(float distance)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_mouse_wheel(distance);
        else
            return false;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_button_down(const mouse_button button, float x, float y)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_mouse_button_down(
                button,
                x - _focused_widget->pos_x(),
                y - _focused_widget->pos_y());
        else
            return false;
    }

	template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_mouse_button_up(
                button,
                x - _focused_widget->pos_x(),
                y - _focused_widget->pos_y());
        else
            return false;
    }

	template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_dbl_click(float x, float y)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_mouse_dbl_click(
                x - _focused_widget->pos_x(),
                y - _focused_widget->pos_y());
        else
            return false;
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_drag_start(const mouse_button button, float x, float y)
    {
        if (_focused_widget) {
            _draging = true;
            return _focused_widget->get()->on_mouse_drag_start(
                button,
                x - _focused_widget->pos_x(),
                y - _focused_widget->pos_y());
        }
        else {
            return false;
        }
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy)
    {
        if (_draging && _focused_widget) {
            return _focused_widget->get()->on_mouse_drag(
                button,
                x - _focused_widget->pos_x(),
                y - _focused_widget->pos_y(),
                dx, dy);
        }
        else {
            return on_mouse_move(x, y);
        }
    }

	template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_drag_end(const mouse_button button, float x, float y)
    {
        if (_draging && _focused_widget) {
            bool used_event = false;
            const auto x_rel = x - _focused_widget->pos_x();
            const auto y_rel = y - _focused_widget->pos_y();

            used_event = _focused_widget->get()->on_mouse_drag_end(button, x_rel, y_rel);

            if (!(_focused_widget->get()->contains(x_rel, y_rel))) {
                used_event |= _focused_widget->get()->on_mouse_exit();
                _focused_widget = nullptr;
            }

            _draging = false;
            return used_event;
        }
        else {
            return false;
        }
    }

    template <typename TDerived, typename TChildren>
    bool widget_container<TDerived, TChildren>::on_mouse_drag_cancel()
    {
        if (_draging && _focused_widget) {
            _draging = false;
            return _focused_widget->get()->on_mouse_drag_cancel();
        }
        else {
            return false;
        }
    }

    template <typename TDerived, typename TChildren>
    void widget_container<TDerived, TChildren>::draw_widgets(cairo_t *cr)
    {
        foreach_holder([cr](widget_holder& holder) {
            cairo_save(cr);
            cairo_translate(cr, holder.pos_x(), holder.pos_y());
            holder.get()->draw(cr);
            cairo_restore(cr);
        });
    }

    template <typename TDerived, typename TChildren>
    void widget_container<TDerived, TChildren>::draw_widgets(cairo_t *cr, const rectangle<>& rect)
    {
        foreach_holder([cr, &rect](widget_holder& holder) {
            const auto child_rect = make_rectangle(
                holder.pos_y(),  holder.pos_y() + holder.get()->height(),
                holder.pos_x(), holder.pos_x() + holder.get()->width());

            rectangle<> drawing_rect;

            //  Redraw only widget that overlap with rect
            if (rect.intersect(child_rect, drawing_rect)) {
                cairo_save(cr);
                cairo_translate(cr, holder.pos_x(), holder.pos_y());
                holder.get()->draw_rect(
                    cr, drawing_rect.translate(-holder.pos_x(), -holder.pos_y()));
                cairo_restore(cr);
            }
        });
    }

}

#endif