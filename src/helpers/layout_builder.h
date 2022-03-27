#ifndef VIEW_LAYOUT_BUILDER_H_
#define VIEW_LAYOUT_BUILDER_H_

#include "../widget_container/border_wrapper.h"
#include "../widget_container/pair_layout.h"
#include "../widget_container/header.h"
#include "../widget_container/background.h"
#include "../widget_container/map_wrapper.h"

namespace View {

    namespace Details
    {
        template <orientation O, typename T>
        auto make_next_widget(float xstep, float ystep, T&& widget)
        {
            if constexpr (O == orientation::horizontal)
                return std::make_unique<border_wrapper>(
                    std::move(widget), 0.f, 0.f, xstep, 0.f);
            else
                return std::make_unique<border_wrapper>(
                    std::move(widget), ystep, 0.f, 0.f, 0.f);
        }

        template <orientation O, bool Frozen, typename ...T>
        struct spaced_layout_builder;

        template <orientation O, bool Frozen, typename T>
        struct spaced_layout_builder<O, Frozen, T>
        {
            static auto build(float xstep, float ystep, T&& child)
            {
                return std::move(child);
            }

            static auto build_tail(float xstep, float ystep, T&& child)
            {
                return make_next_widget<O>(xstep, ystep, std::move(child));
            }
        };

        template <orientation O, bool Frozen, typename T, typename ...Ts>
        struct spaced_layout_builder<O, Frozen, T, Ts...>
        {
            static auto build(float xstep, float ystep, T&& first, Ts&& ...lasts)
            {
                return make_layout<O, Frozen>(
                    std::move(first),
                    spaced_layout_builder<O, Frozen, Ts...>::build_tail(
                        xstep, ystep, std::move(lasts)...));
            }

            static auto build_shared(float xstep, float ystep, T&& first, Ts&& ...lasts)
            {
                return make_shared_layout<O, Frozen>(
                    std::move(first),
                    spaced_layout_builder<O, Frozen, Ts...>::build_tail(
                        xstep, ystep, std::move(lasts)...));
            }

            static auto build_tail(float xstep, float ystep, T&& first, Ts&& ...lasts)
            {
                return make_layout<O, Frozen>(
                    make_next_widget<O>(xstep, ystep, std::move(first)),
                    spaced_layout_builder<O, Frozen, Ts...>::build_tail(
                        xstep, ystep, std::move(lasts)...));
            }
        };
    }

    class layout_builder
    {
    public:
        layout_builder(
            float horizontal_step = 3.f,
            float vertical_step = 3.f) noexcept;

        template <bool Frozen = true, typename ...T>
        auto horizontal(T&& ...childs) const
        {
            return Details::spaced_layout_builder<orientation::horizontal, Frozen, T...>::build(
                _horizontal_step, _vertical_step, std::move(childs)...);
        }

        template <bool Frozen = true, typename ...T>
        auto shared_horizontal(T&& ...childs) const
        {
            return Details::spaced_layout_builder<orientation::horizontal, Frozen, T...>::build_shared(
                _horizontal_step, _vertical_step, std::move(childs)...);
        }

        template <bool Frozen = true, typename ...T>
        auto vertical(T&& ...childs) const
        {
            return Details::spaced_layout_builder<orientation::vertical, Frozen, T...>::build(
                _horizontal_step, _vertical_step, std::move(childs)...);
        }

        template <bool Frozen = true, typename ...T>
        auto shared_vertical(T&& ...childs) const
        {
            return Details::spaced_layout_builder<orientation::vertical, Frozen, T...>::build_shared(
                _horizontal_step, _vertical_step, std::move(childs)...);
        }

        std::unique_ptr<View::header> header(
            std::unique_ptr<widget>&& child,
            color_theme::color background = color_theme::color::SURFACE,
            float internal_border_size = 5.f,
            float header_size = 16.f,
            float border_size = 3.f) const;

        std::shared_ptr<View::header> shared_header(
            std::unique_ptr<widget>&& child,
            color_theme::color background = color_theme::color::SURFACE,
            float header_size = 5.f,
            float border_size = 16.f,
            float internal_border_size = 3.f) const;

        std::unique_ptr<map_wrapper> map(std::unique_ptr<widget>&& child, float width = 100.f, float height = 100.f) const;
        std::shared_ptr<map_wrapper> shared_map(std::unique_ptr<widget>&& child, float width = 100.f, float height = 100.f) const;

        std::unique_ptr<background> windows(std::unique_ptr<widget>&& child, float border_width = 3.f, float border_heith = 3.f) const;
        std::unique_ptr<widget> empty_space(
            float width = 1.f, float height = 1.f,
            size_constraint width_constraint = free_size, size_constraint height_constraint = free_size) const;

    private:
        const float _horizontal_step;
        const float _vertical_step;
    };
}

#endif