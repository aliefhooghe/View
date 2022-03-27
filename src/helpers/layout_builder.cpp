
#include "layout_builder.h"

namespace View
{
    layout_builder::layout_builder(
        float horizontal_step,
        float vertical_step) noexcept
    :    _horizontal_step{horizontal_step},
        _vertical_step{vertical_step}
    {
    }

    std::unique_ptr<View::header> layout_builder::header(std::unique_ptr<widget>&& child, color_theme::color background, float internal_border_size, float header_size, float border_size) const
    {
        return std::make_unique<View::header>(std::move(child), background, header_size, border_size, internal_border_size);
    }

    std::shared_ptr<View::header> layout_builder::shared_header(std::unique_ptr<widget>&& child, color_theme::color background, float internal_border_size, float header_size, float border_size) const
    {
        return std::make_shared<View::header>(std::move(child), background, header_size, border_size, internal_border_size);
    }

    std::unique_ptr<map_wrapper> layout_builder::map(std::unique_ptr<widget>&& child, float width, float height) const
    {
        return std::make_unique<map_wrapper>(std::move(child), width, height);
    }

    std::shared_ptr<map_wrapper> layout_builder::shared_map(std::unique_ptr<widget>&& child, float width, float height) const
    {
        return std::make_shared<map_wrapper>(std::move(child), width, height);
    }

    std::unique_ptr<background> layout_builder::windows(std::unique_ptr<widget>&& child, float border_width, float border_heith) const
    {
        return std::make_unique<View::background>(
                     std::make_unique<border_wrapper>(
                        std::move(child),
                        border_heith, border_heith,
                        border_width, border_width));
    }

    std::unique_ptr<widget> layout_builder::empty_space(float width, float height, size_constraint width_constraint, size_constraint height_constraint) const
    {
        return std::make_unique<widget>(width, height, width_constraint, height_constraint);
    }
}