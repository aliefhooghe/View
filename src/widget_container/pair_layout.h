#ifndef VIEW_PAIR_LAYOUT_H_
#define VIEW_PAIR_LAYOUT_H_

#include "panel.h"
#include "layout_separator.h"

namespace View {

    template <orientation Orientation /* Separator Orientation */>
    class pair_layout : public panel_implementation<> {
        static constexpr auto _separator_orthogonal_size = 4.0f;

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
        void resize_clamp_delta(widget* p, float& delta)
        {
            const auto& size_constraint = get_size_constrain<O>(p);
            const auto size = widget_size<O>(p);
            resize<O>(p, size_constraint.clamp_delta(size, delta));
        }

        auto layout_width(widget* pleft, widget* pright)
        {
            if constexpr (Orientation == orientation::horizontal)
                return std::max(pleft->width(), pright->width());
            else
                return pleft->width() + pright->width();
        }

        auto layout_height(widget* pleft, widget* pright)
        {
            if constexpr (Orientation == orientation::horizontal)
                return pleft->height() + pright->height();
            else
                return std::max(pleft->height(), pright->height());
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
            //  Size along 'Orientation'
            const auto orientation_size = widget_size<Orientation>(this);
            const auto first_width = widget_size<orthogonal(Orientation)>(first.get());

            resize<Orientation>(first.get(), orientation_size);
            resize<Orientation>(second.get(), orientation_size);

            //  Compute and set layout size constraints
            if constexpr (Orientation == orientation::horizontal) {
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
            set_position<orthogonal(Orientation)>(_second(), first_width);

            //Insert Separator
            auto separator =
                std::make_unique<layout_separator>(0.f, 0.f, Orientation);

            separator->set_callback(
                [this](float delta) {
                    const auto target_first_orthogonal_size = widget_size<orthogonal(Orientation)>(_first().get()) + delta;
                    const auto target_second_orthogonal_size = widget_size<orthogonal(Orientation)>(_second().get()) - delta;

                    if (match_size_constrain<orthogonal(Orientation)>(_first().get(), target_first_orthogonal_size) &&
                        match_size_constrain<orthogonal(Orientation)>(_second().get(), target_second_orthogonal_size))
                    {
                        //  Move/Resize widgets
                        resize<orthogonal(Orientation)>(_first().get(), target_first_orthogonal_size);
                        resize<orthogonal(Orientation)>(_second().get(), target_second_orthogonal_size);
                        set_position<orthogonal(Orientation)>(_second(), target_first_orthogonal_size);

                        //  Move Separator
                        set_position<orthogonal(Orientation)>(
                            _separator(),
                            std::clamp(
                                target_first_orthogonal_size - _separator_orthogonal_size / 2.f,
                                0.f, widget_size<orthogonal(Orientation)>(this)));

                        //  redraw
                        invalidate();
                    }
                });


            resize<Orientation>(separator.get(), orientation_size, _separator_orthogonal_size);
            insert_widget(0.f, 0.f, std::move(separator));
            set_position<orthogonal(Orientation)>(_separator(), first_width - _separator_orthogonal_size / 2.f);
        }

        ~pair_layout() override = default;

        bool resize(float width, float height) override
        {
            if (width_constraint().contains(width) &&
                height_constraint().contains(height))
            {
                const auto target_orientation_size = choose_dim<Orientation>(width, height);
                const auto target_orthogonal_size = choose_dim<orthogonal(Orientation)>(width, height);

                if (target_orthogonal_size != widget_size<orthogonal(Orientation)>(this)) {
                    auto orthogonal_delta = target_orthogonal_size - widget_size<orthogonal(Orientation)>(this);

                    //  Resize second as much as possible and then first
                    resize_clamp_delta<orthogonal(Orientation)>(_second().get(), orthogonal_delta);
                    resize_clamp_delta<orthogonal(Orientation)>(_first().get(), orthogonal_delta);

                    //  Second and separator need to be moved
                    const auto first_orthogonal_size = widget_size<orthogonal(Orientation)>(_first().get());
                    set_position<orthogonal(Orientation)>(_second(), first_orthogonal_size);
                    set_position<orthogonal(Orientation)>(_separator(), first_orthogonal_size - _separator_orthogonal_size / 2.f);
                }

                //  Resize along Orientation
                resize<Orientation>(_first().get(), target_orientation_size);
                resize<Orientation>(_second().get(), target_orientation_size);
                resize<Orientation>(_separator().get(), target_orientation_size);


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

    using vertical_pair_layout = pair_layout<orientation::horizontal>;
    using horizontal_pair_layout = pair_layout<orientation::vertical>;

}

#endif