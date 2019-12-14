#ifndef VIEW_WIDGET_CONTAINER_H_
#define VIEW_WIDGET_CONTAINER_H_


#include <vector>
#include <memory>
#include <algorithm>

#include "widget.h"
#include "display/common/display_controler.h"

namespace View {

    template <typename Tchildren = widget>
    class widget_container : public widget {

        class widget_holder : private display_controler {

        public:
            widget_holder(
                widget_container& parent,
                float x, float y, std::unique_ptr<Tchildren>&& w)
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
            std::unique_ptr<Tchildren> _widget_instance;
            widget_container& _parent;
        };

    public:
        widget_container(float width, float height)
        : widget(width, height)
        {}

        ~widget_container() override = default;

        //  Container Api
        void insert_widget(float x, float y, std::unique_ptr<Tchildren>&& w)
        {
            _childrens.emplace_back(*this, x, y, std::move(w));
        }

        void remove_widget(Tchildren *children)
        {
            _childrens.erase(std::remove_if(
                _childrens.begin(), _childrens.end(),
                [children](const auto& holder)
                {
                    return holder.is(children);
                }),
                _childrens.end());

            if (_focused_widget == children)
                _focused_widget = nullptr;
        }

        //  Event implementations
        bool on_key_up(const keycode key) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_key_up(key);
            else
                return false;
        }

        bool on_key_down(const keycode key) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_key_up(key);
            else
                return false;
        }

        bool on_mouse_enter() override
        {
            return false;
        }

        bool on_mouse_exit() override
        {
            if (_focused_widget) {
                if (_draging)
                    _focused_widget->get()->on_mouse_drag_cancel();
                _focused_widget->get()->on_mouse_exit();
                _focused_widget = nullptr;
            }

            return true;
        }

        bool on_mouse_move(float x, float y) override
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

        bool on_mouse_wheel(float distance) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_mouse_wheel(distance);
            else
                return false;
        }

        bool on_mouse_button_down(const mouse_button button, float x, float y) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_mouse_button_down(
                    button,
                    x - _focused_widget->pos_x(),
                    y - _focused_widget->pos_y());
            else
                return false;
        }

		bool on_mouse_button_up(const mouse_button button, float x, float y) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_mouse_button_up(
                    button,
                    x - _focused_widget->pos_x(),
                    y - _focused_widget->pos_y());
            else
                return false;
        }

		bool on_mouse_dbl_click(float x, float y) override
        {
            if (_focused_widget)
                return _focused_widget->get()->on_mouse_dbl_click(
                    x - _focused_widget->pos_x(),
                    y - _focused_widget->pos_y());
            else
                return false;
        }

        bool on_mouse_drag_start(const mouse_button button, float x, float y) override
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

        bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy) override
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

		bool on_mouse_drag_end(const mouse_button button, float x, float y) override
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

        bool on_mouse_drag_cancel() override
        {
            if (_draging && _focused_widget) {
                _draging = false;
                return _focused_widget->get()->on_mouse_drag_cancel();
            }
            else {
                return false;
            }
        }

        void draw(cairo_t *cr) override
        {
            //  First draw background
            draw_background(cr);

            //  Then draw childrens
            for (auto& w_holder : _childrens) {
                cairo_save(cr);
                cairo_translate(cr, w_holder.pos_x(), w_holder.pos_y());
                w_holder.get()->draw(cr);
                cairo_restore(cr);
            }

            //  Finally draw foreground
            draw_foreground(cr);
        }

        void draw_rect(cairo_t *cr, const rectangle<>& rect) override
        {
            draw_background(cr);

            for (auto& w_holder : _childrens) {
                const auto child_rect = make_rectangle(
                    w_holder.pos_y(),  w_holder.pos_y() + w_holder.get()->height(),
                    w_holder.pos_x(), w_holder.pos_x() + w_holder.get()->width());

                rectangle<> drawing_rect;

                //  Redraw only widget that overlap with rect
                if (rect.intersect(child_rect, drawing_rect)) {
                    cairo_save(cr);
                    cairo_translate(cr, w_holder.pos_x(), w_holder.pos_y());
                    w_holder.get()->draw_rect(
                        cr, drawing_rect.translate(-w_holder.pos_x(), -w_holder.pos_y()));
                    cairo_restore(cr);
                }
            }

            draw_foreground(cr);
        }

    protected:
        //  Drawing can be specialized
        virtual void draw_background(cairo_t*) { }
        virtual void draw_foreground(cairo_t*) { }

        //  Helpers
        widget_holder *focused_widget()     { return _focused_widget; }
        widget_holder *widget_at(float x, float y)
        {
            //  Last widget are in foreground

            // TODO : Optimize (kd-tree ?)
            for (auto it = _childrens.rbegin(); it != _childrens.rend(); ++it) {
                const auto x_rel = x - it->pos_x();
                const auto y_rel = y - it->pos_y();

                if (it->get()->contains(x_rel, y_rel))
                    return &(*it);
            }

            //  No widget at this position
            return nullptr;
        }

    private:
        std::vector<widget_holder> _childrens;
        widget_holder* _focused_widget{ nullptr };
        bool _draging{false};
    };

}

#endif