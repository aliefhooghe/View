#ifndef VIEW_PAIR_LAYOUT_H_
#define VIEW_PAIR_LAYOUT_H_

#include "widget_container.h"
#include "layout_separator.h"

namespace View {

    template <orientation Orientation>
    class pair_layout : public widget_container<pair_layout<Orientation>> {
        friend class widget_container<pair_layout<Orientation>>;

        static constexpr auto _separator_width = 1.f;

        /**
         * Orientation abstraction helpers
         **/
        template <orientation O>
        auto widget_size(widget* p)
        {
            if constexpr (O == orientation::horizontal)
                return p->width();
            else
                return p->height();
        }

        template <orientation O>
        auto widget_pos(widget_holder<>& w)
        {
            if constexpr (O == orientation::horizontal)
                return w.pos_x();
            else
                return w.pos_y();
        }

        template <orientation O>
        auto resize(widget *p, float new_size)
        {
            if constexpr (O == orientation::horizontal)
                return p->resize_width(new_size);
            else
                return p->resize_height(new_size);
        }

        template <orientation O>
        auto resize(widget *p, float orientation_size, float orthogonal_size)
        {
            if constexpr (O == orientation::horizontal)
                return p->resize(orientation_size, orthogonal_size);
            else
                return p->resize(orthogonal_size, orientation_size);
        }

        template <orientation O>
        const auto& get_size_constrain(widget* p)
        {
            if constexpr (O == orientation::horizontal)
                return p->width_constraint();
            else
                return p->height_constraint();
        }

        template <orientation O>
        const auto match_size_constrain(widget* p, float size)
        {
            const auto& constrain = get_size_constrain<O>(p);
            return constrain.contains(size);
        }

        template <orientation O>
        void resize_clamp(widget* p, float size)
        {
            const auto& size_constraint = get_size_constrain<O>(p);
            resize<O>(p, size_constraint.clamp(size));
        }

        template <orientation O>
        void resize_clamp_delta(widget* p, float& delta)
        {
            const auto& size_constraint = get_size_constrain<O>(p);
            const auto size = widget_size<O>(p);
            resize<O>(p, size_constraint.clamp_delta(size, delta));
        }

        auto layout_width(widget* pleft, widget* pright)
        {
            if constexpr (Orientation == orientation::horizontal)
                return pleft->width() + pright->width();
            else
                return std::max(pleft->width(), pright->width());
        }

        auto layout_height(widget* pleft, widget* pright)
        {
            if constexpr (Orientation == orientation::horizontal)
                return std::max(pleft->height(), pright->height());
            else
                return pleft->height() + pright->height();
        }

        template <orientation O>
        auto set_position(widget_holder<>& w, float pos)
        {
            if constexpr (O == orientation::horizontal)
                return w.set_pos_x(pos);
            else
                return w.set_pos_y(pos);
        }

        template <orientation O>
        constexpr auto choose_dim(float width, float height)
        {
            if constexpr (O == orientation::horizontal)
                return width;
            else
                return height;
        }

    public:
        pair_layout(std::unique_ptr<widget>&& first, std::unique_ptr<widget>&& second)
        : widget_container<pair_layout<Orientation>>{layout_width(first.get(), second.get()), layout_height(first.get(), second.get())},
            _first{*this, 0.f, 0.f, std::move(first)},
            _second{*this, 0.f, 0.f, std::move(second)},
            _separator{*this, 0.f, 0.f}
        {
            const auto orientation_size = widget_size<Orientation>(this);
            const auto orthogonal_size = widget_size<orthogonal(Orientation)>(this);
            const auto first_orientation_size = widget_size<Orientation>(_first.get());

            //  Compute and set layout size constraints
            if constexpr (Orientation == orientation::vertical) {
                widget_container<pair_layout<Orientation>>::set_size_constraints(
                    _first->width_constraint().intersect(_second->width_constraint()),
                    _first->height_constraint() + _second->height_constraint());
            }
            else {
                widget_container<pair_layout<Orientation>>::set_size_constraints(
                    _first->width_constraint() + _second->width_constraint(),
                    _first->height_constraint().intersect(_second->height_constraint()));
            }

            //  Create separator
            auto separator = std::make_unique<layout_separator>(0.f, 0.f, orthogonal(Orientation));

            separator->set_callback(
                [this](float delta) {
                    const auto target_first_orientation_size = widget_size<Orientation>(_first.get()) + delta;
                    const auto target_second_orientation_size = widget_size<Orientation>(_second.get()) - delta;

                    if (match_size_constrain<Orientation>(_first.get(), target_first_orientation_size) &&
                        match_size_constrain<Orientation>(_second.get(), target_second_orientation_size))
                    {
                        //  Move/Resize widgets
                        resize<Orientation>(_first.get(), target_first_orientation_size);
                        resize<Orientation>(_second.get(), target_second_orientation_size);
                        set_position<Orientation>(_second, target_first_orientation_size);

                        //  Move Separator
                        set_position<Orientation>(
                            _separator,
                            std::clamp(
                                target_first_orientation_size - _separator_width / 2.f,
                                0.f, widget_size<Orientation>(this)));

                        //  redraw
                        widget_container<pair_layout<Orientation>>::invalidate();
                    }
                });

            _separator.set_widget(std::move(separator));

            //  Set childrens sizes
            resize_clamp<orthogonal(Orientation)>(_first.get(), orthogonal_size);
            resize_clamp<orthogonal(Orientation)>(_second.get(), orthogonal_size);
            resize<Orientation>(_separator.get(), _separator_width, orthogonal_size);

            //  Set childrens positions
            set_position<Orientation>(_second, first_orientation_size);
            set_position<Orientation>(_separator, first_orientation_size - _separator_width / 2.f);
        }

        ~pair_layout() override = default;

        bool resize(float width, float height) override
        {
            const auto width_constraint = widget_container<pair_layout<Orientation>>::width_constraint();
            const auto height_constraint = widget_container<pair_layout<Orientation>>::height_constraint();

            if (width_constraint.contains(width) &&
                height_constraint.contains(height))
            {
                const auto current_orientation_size = widget_size<Orientation>(this);
                const auto target_orientation_size = choose_dim<Orientation>(width, height);
                const auto target_orthogonal_size = choose_dim<orthogonal(Orientation)>(width, height);

                if (target_orientation_size != current_orientation_size) {
                    auto orientation_delta = target_orientation_size - current_orientation_size;

                    //  Resize second as much as possible and then first
                    resize_clamp_delta<Orientation>(_second.get(), orientation_delta);
                    resize_clamp_delta<Orientation>(_first.get(), orientation_delta);

                    //  Update second and separator position
                    const auto first_orientation_size = widget_size<Orientation>(_first.get());
                    set_position<Orientation>(_second, first_orientation_size);
                    set_position<Orientation>(_separator, first_orientation_size - _separator_width / 2.f);
                }

                //  Resize along Orientation
                resize_clamp<orthogonal(Orientation)>(_first.get(), target_orthogonal_size);
                resize_clamp<orthogonal(Orientation)>(_second.get(), target_orthogonal_size);
                resize_clamp<orthogonal(Orientation)>(_separator.get(), target_orthogonal_size);

                widget_container<pair_layout<Orientation>>::resize(width, height);

                return true;
            }
            else {
                return false;
            }
        }

        void draw(NVGcontext *vg) override
        {
            widget_container<pair_layout<Orientation>>::draw_widgets(vg);
        }

    private:

        auto widget_at(float x, float y)
        {
            const auto orientation_cursor = choose_dim<Orientation>(x, y);
            //const auto orthogonal_cursor = choose_dim<orthogonal(Orientation)>(x, y);
            const auto delta = widget_pos<Orientation>(_separator) - orientation_cursor;

            if (std::abs(delta) < (_separator_width / 2.f))
                return &_separator;
            else if (delta > 0)
                return &_first;
            else
                return &_second;
        }

        template <typename TFunction>
        void foreach_holder(TFunction func)
        {
            func(_first);
            func(_second);
            func(_separator);
        }

        widget_holder<> _first;
        widget_holder<> _second;
        widget_holder<> _separator;
    };

    /**
     *  \brief Aliases for pair_layout
     *  \details A vertical layout have horizontal separator and vice versa
     **/
    using horizontal_pair_layout = pair_layout<orientation::horizontal>;
    using vertical_pair_layout = pair_layout<orientation::vertical>;

    template <orientation O, typename T, typename ...Ts>
    auto make_layout(T&& first, Ts ...next)
    {
        if constexpr (sizeof...(Ts) == 0)
            return std::move(first);
        else
            return std::make_unique<pair_layout<O>>(
                std::move(first), make_layout<O>(std::move(next)...));
    }

    template <typename ...T>
    auto make_horizontal_layout(T&& ...childs)
    {
        return make_layout<orientation::horizontal>(std::move(childs)...);
    }

    template <typename ...T>
    auto make_vertical_layout(T&& ...childs)
    {
        return make_layout<orientation::vertical>(std::move(childs)...);
    }

}

#endif