#ifndef VIEW_PAIR_LAYOUT_H_
#define VIEW_PAIR_LAYOUT_H_

#include "panel.h"
#include "layout_separator.h"

namespace View {

    template <orientation Orientation>
    class pair_layout : public panel_implementation<> {
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
        : panel_implementation<>{layout_width(first.get(), second.get()), layout_height(first.get(), second.get())}
        {
            const auto orientation_size = widget_size<Orientation>(this);
            const auto orthogonal_size = widget_size<orthogonal(Orientation)>(this);
            const auto first_orientation_size = widget_size<Orientation>(first.get());

            resize<orthogonal(Orientation)>(first.get(), orthogonal_size);
            resize<orthogonal(Orientation)>(second.get(), orthogonal_size);

            //  Compute and set layout size constraints
            if constexpr (Orientation == orientation::vertical) {
                set_size_constraints(
                    first->width_constraint().intersect(second->width_constraint()),
                    first->height_constraint() + second->height_constraint());
            }
            else {
                set_size_constraints(
                    first->width_constraint() + second->width_constraint(),
                    first->height_constraint().intersect(second->height_constraint()));
            }

            //  Insert childrens
            insert_widget(0.f, 0.f, std::move(first));
            insert_widget(0.f, 0.f, std::move(second));
            set_position<Orientation>(_second(), first_orientation_size);

            //Insert Separator
            auto separator =
                std::make_unique<layout_separator>(0.f, 0.f, orthogonal(Orientation));

            separator->set_callback(
                [this](float delta) {
                    const auto target_first_orientation_size = widget_size<Orientation>(_first().get()) + delta;
                    const auto target_second_orientation_size = widget_size<Orientation>(_second().get()) - delta;

                    if (match_size_constrain<Orientation>(_first().get(), target_first_orientation_size) &&
                        match_size_constrain<Orientation>(_second().get(), target_second_orientation_size))
                    {
                        //  Move/Resize widgets
                        resize<Orientation>(_first().get(), target_first_orientation_size);
                        resize<Orientation>(_second().get(), target_second_orientation_size);
                        set_position<Orientation>(_second(), target_first_orientation_size);

                        //  Move Separator
                        set_position<Orientation>(
                            _separator(),
                            std::clamp(
                                target_first_orientation_size - _separator_width / 2.f,
                                0.f, widget_size<Orientation>(this)));

                        //  redraw
                        invalidate();
                    }
                });


            resize<Orientation>(separator.get(), _separator_width, orthogonal_size);
            insert_widget(0.f, 0.f, std::move(separator));
            set_position<Orientation>(_separator(), first_orientation_size - _separator_width / 2.f);
        }

        ~pair_layout() override = default;

        bool resize(float width, float height) override
        {
            if (width_constraint().contains(width) &&
                height_constraint().contains(height))
            {
                const auto current_orientation_size = widget_size<Orientation>(this);
                const auto target_orientation_size = choose_dim<Orientation>(width, height);
                const auto target_orthogonal_size = choose_dim<orthogonal(Orientation)>(width, height);

                if (target_orientation_size != current_orientation_size) {
                    auto orientation_delta = target_orientation_size - current_orientation_size;

                    //  Resize second as much as possible and then first
                    resize_clamp_delta<Orientation>(_second().get(), orientation_delta);
                    resize_clamp_delta<Orientation>(_first().get(), orientation_delta);

                    //  Update second and separator position
                    const auto first_orientation_size = widget_size<Orientation>(_first().get());
                    set_position<Orientation>(_second(), first_orientation_size);
                    set_position<Orientation>(_separator(), first_orientation_size - _separator_width / 2.f);
                }

                //  Resize along Orientation
                resize_clamp<orthogonal(Orientation)>(_first().get(), target_orthogonal_size);
                resize_clamp<orthogonal(Orientation)>(_second().get(), target_orthogonal_size);
                resize_clamp<orthogonal(Orientation)>(_separator().get(), target_orthogonal_size);

                panel_implementation<>::resize(width, height);

                return true;
            }
            else {
                return false;
            }
        }

    private:
        auto& _first() noexcept { return _childrens[0]; }
        auto& _second() noexcept { return _childrens[1]; }
        auto& _separator() noexcept { return _childrens[2]; }
    };

    /**
     *  \brief Aliases for pair_layout
     *  \details A vertical layout have horizontal separator and vice versa
     **/
    using horizontal_pair_layout = pair_layout<orientation::horizontal>;
    using vertical_pair_layout = pair_layout<orientation::vertical>;


    template <typename T, typename ...Ts>
    auto make_vertical_layout(T&& first, Ts&& ...nexts)
    {
        if constexpr (sizeof...(Ts) == 0)
            return std::move(first);
        else
            return std::make_unique<vertical_pair_layout>(
                        std::move(first), make_vertical_layout(std::move(nexts)...));
    }

    template <typename T, typename ...Ts>
    auto make_horizontal_layout(T&& first, Ts&& ...nexts)
    {
        if constexpr (sizeof...(Ts) == 0)
            return std::move(first);
        else
            return std::make_unique<horizontal_pair_layout>(
                        std::move(first), make_horizontal_layout(std::move(nexts)...));
    }

}

#endif