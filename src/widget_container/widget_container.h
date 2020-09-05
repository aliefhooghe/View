#ifndef VIEW_WIDGET_CONTAINER_H_
#define VIEW_WIDGET_CONTAINER_H_

#include <memory>

#include "widget/widget.h"
#include "display/common/display_controler.h"

namespace View {

    template <typename TChildren = widget>
    class widget_holder : private display_controler {

    public:
        widget_holder(widget& parent,
            float x, float y, std::unique_ptr<TChildren>&& w)
        :   _parent{&parent}, display_controler{*w},
            _pos_x{x}, _pos_y{y}, _widget_instance{std::move(w)}
        {}

        widget_holder(widget& parent, float x, float y)
        :   _parent{&parent},
            _pos_x{x}, _pos_y{y}
        {}

        widget_holder(widget_holder&& other) noexcept
        :   display_controler{*other._widget_instance},
            _parent{other._parent},
            _pos_x{other._pos_x}, _pos_y{other._pos_y},
            _widget_instance{std::move(other._widget_instance)}
        {}

        widget_holder(const widget_holder& x) noexcept = delete;
        virtual ~widget_holder() = default;

        widget_holder& operator=(widget_holder&& other) noexcept
        {
            _parent = other._parent;
            _pos_x = other._pos_x;
            _pos_y = other._pos_y;
            set_widget(std::move(other._widget_instance));
            return *this;
        }

        void set_widget(std::unique_ptr<TChildren>&& w)
        {
            _widget_instance = std::move(w);
            display_controler::set_widget(*_widget_instance);   //  assign this widget to the display_ctl interface
        }

        void set_pos(float x, float y) noexcept { _pos_x = x; _pos_y = y; }
        void set_pos_x(float x) noexcept { _pos_x = x; }
        void set_pos_y(float y) noexcept { _pos_y = y; }

        auto pos_x() const noexcept { return _pos_x; }
        auto pos_y() const noexcept { return _pos_y; }

        auto get() noexcept { return _widget_instance.get(); }
        auto get() const noexcept { return _widget_instance.get(); }

        auto operator->() noexcept { return get(); }
        auto operator->() const noexcept { return get(); }

    protected:
        /**
         *      Display controler interface
         */
        void invalidate_rect(const rectangle<>& rect) override
        {
            _parent->invalidate_rect(rect.translate(_pos_x, _pos_y));
        }

        void set_cursor(cursor c) override
        {
            _parent->set_cursor(c);
        }

        float widget_pos_x() override
        {
            return _pos_x;
        }

        float widget_pos_y() override
        {
            return _pos_y;
        }

        float _pos_x;
        float _pos_y;
        std::unique_ptr<TChildren> _widget_instance{};
        widget *_parent;
    };

    template <typename TDerived, typename TChildren = widget>
    class widget_container : public widget {

    public:
        widget_container(float width, float height) noexcept
        : widget{width, height}
        {}
        widget_container(float width, float height, size_constraint width_constraint, size_constraint height_constraint) noexcept
        : widget{width, height, width_constraint, height_constraint}
        {}
        ~widget_container() override = default;

        //  Events
        bool on_key_up(const keycode key) override;
        bool on_key_down(const keycode key) override;
        bool on_text_input(std::string_view txt) override;
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

        void apply_color_theme(const color_theme& theme) override;

    protected:
        void draw_widgets(NVGcontext *vg);
        void draw_widgets(NVGcontext *vg, const rectangle<>& rect);

        auto focused_widget() const noexcept { return _focused_widget; }
        void reset_focused_widget() noexcept { _focused_widget = nullptr; }

    private:
        widget_holder<TChildren> *widget_at(float x, float y)
        {
            return static_cast<TDerived*>(this)->widget_at(x, y);
        }

        template <typename TFunction>
        void foreach_holder(TFunction func)
        {
            static_cast<TDerived*>(this)->foreach_holder(func);
        }

        widget_holder<TChildren>* _focused_widget{ nullptr };
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
    bool  widget_container<TDerived, TChildren>::on_text_input(std::string_view txt)
    {
        if (_focused_widget)
            return _focused_widget->get()->on_text_input(txt);
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
            auto *child = widget_at(x, y);

            if (child == _focused_widget) {
                const auto x_rel = x - _focused_widget->pos_x();
                const auto y_rel = y - _focused_widget->pos_y();
                return _focused_widget->get()->on_mouse_move(x_rel, y_rel);
            }
            else {
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
        if (_draging) {
            bool used_event = false;

            if (_focused_widget != nullptr) {
                const auto x_rel = x - _focused_widget->pos_x();
                const auto y_rel = y - _focused_widget->pos_y();
                used_event |= _focused_widget->get()->on_mouse_drag_end(button, x_rel, y_rel);
            }

            auto *child = widget_at(x, y);

            if (child != _focused_widget) {
                if (child != nullptr)
                    used_event |= child->get()->on_mouse_enter();
                if (_focused_widget != nullptr)
                    used_event |= _focused_widget->get()->on_mouse_exit();
            }

            _focused_widget = child;
            _draging = false;
            return used_event;
        }
        else
        {
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
    void widget_container<TDerived, TChildren>::apply_color_theme(const color_theme& theme)
    {
        foreach_holder([&theme](auto& holder) {
            holder->apply_color_theme(theme);
        });
    }

    template <typename TDerived, typename TChildren>
    void widget_container<TDerived, TChildren>::draw_widgets(NVGcontext *vg)
    {
        foreach_holder([vg](auto& holder) {
            nvgSave(vg);
            nvgTranslate(vg, holder.pos_x(), holder.pos_y());
            holder->draw(vg);
            nvgRestore(vg);
        });
    }

    template <typename TDerived, typename TChildren>
    void widget_container<TDerived, TChildren>::draw_widgets(NVGcontext *vg, const rectangle<>& rect)
    {
        foreach_holder([vg, &rect](auto& holder) {
            const auto child_rect = make_rectangle(
                holder.pos_y(), holder.pos_y() + holder->height(),
                holder.pos_x(), holder.pos_x() + holder->width());

            rectangle<> drawing_rect;

            //  Redraw only widget that overlap with rect
            if (rect.intersect(child_rect, drawing_rect)) {
                nvgSave(vg);
                nvgTranslate(vg, holder.pos_x(), holder.pos_y());
                holder.get()->draw_rect(
                    vg, drawing_rect.translate(-holder.pos_x(), -holder.pos_y()));
                nvgRestore(vg);
            }
        });
    }

}

#endif