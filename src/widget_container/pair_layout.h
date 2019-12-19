#ifndef VIEW_PAIR_LAYOUT_H_
#define VIEW_PAIR_LAYOUT_H_

#include "panel.h"
#include "layout_separator.h"

namespace View {

    template <orientation Orientation /* Separator Orientation */>
    class pair_layout : public panel<> {

        using widget_holder = panel<>::widget_holder;
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
        auto match_size_constrain(widget* p, float size)
        {
            if constexpr (O == orientation::horizontal)
                return p->width_constraint().contains(size);
            else
                return p->height_constraint().contains(size);
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
        auto set_position(widget_holder& w, float pos)
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
        : panel<>{layout_width(first.get(), second.get()), layout_height(first.get(), second.get())}
        {
            //  Size along 'Orientation'
            const auto orientation_size = widget_size<Orientation>(this);

            _border = widget_size<orthogonal(Orientation)>(first.get());

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
            set_position<orthogonal(Orientation)>(_second(), _border);

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
            set_position<orthogonal(Orientation)>(_separator(), _border - _separator_orthogonal_size / 2.f);
        }

        ~pair_layout() override = default;

        bool resize(float width, float height) override
        {
            if (width_constraint().contains(width) &&
                height_constraint().contains(height))
            {
                const auto target_orientation_size = choose_dim<Orientation>(width, height);
                const auto target_orthogonal_size = choose_dim<orthogonal(Orientation)>(width, height);

                auto first_orthogonal_size = widget_size<orthogonal(Orientation)>(_first().get());
                auto second_orthogonal_size = widget_size<orthogonal(Orientation)>(_second().get());

                const auto initial_second_orthogonal_size = second_orthogonal_size;

                if (target_orthogonal_size != widget_size<orthogonal(Orientation)>(this)) {
                    const auto orthogonal_delta =
                        target_orthogonal_size - widget_size<orthogonal(Orientation)>(this);
                    const auto target_second_orthogonal_size =
                        second_orthogonal_size + orthogonal_delta;

                    //  First step : try to resize second widget
                    if (match_size_constrain<orthogonal(Orientation)>(_second().get(), target_second_orthogonal_size)) {
                        second_orthogonal_size = target_second_orthogonal_size;
                    }
                    else {  //  If we can't, try first widget
                        const auto target_first_orthogonal_size =
                            first_orthogonal_size + orthogonal_delta;

                        if (match_size_constrain<orthogonal(Orientation)>(_first().get(), target_first_orthogonal_size)) {
                            first_orthogonal_size = target_first_orthogonal_size;
                            //  Second and separator need to be moved when fisrt is resized
                            set_position<orthogonal(Orientation)>(_second(), target_first_orthogonal_size);
                            set_position<orthogonal(Orientation)>(_separator(), target_first_orthogonal_size - _separator_orthogonal_size / 2.f);
                        }
                    }
                }

                //  Set new sizes
                resize<Orientation>(_first().get(), target_orientation_size, first_orthogonal_size);
                resize<Orientation>(_second().get(), target_orientation_size, second_orthogonal_size);
                resize<Orientation>(_separator().get(), target_orientation_size);

                panel<>::resize(width, height);

                return true;
            }
            else {
                return false;
            }
        }

    private:
        widget_holder& _first() noexcept { return _childrens[0]; }
        widget_holder& _second() noexcept { return _childrens[1]; }
        widget_holder& _separator() noexcept { return _childrens[2]; }
        float _border;
    };

    /**
     *  \brief Aliases for pair_layout
     *  \details A vertical layout have horizontal separator and vice versa
     **/

    using vertical_pair_layout = pair_layout<orientation::horizontal>;
    using horizontal_pair_layout = pair_layout<orientation::vertical>;

}

#endif