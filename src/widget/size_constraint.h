#ifndef VIEW_SIZE_CONSTRAINT_H_
#define VIEW_SIZE_CONSTRAINT_H_

#include <algorithm>
#include <variant>

namespace View {

    struct size_constraint
    {

        bool contains(float x) const noexcept
        {
            return (x >= min && x <= max);
        }

        auto operator +(const size_constraint& other) const noexcept
        {
            return size_constraint{min + other.min, max + other.max};
        }

        auto intersectc(const size_constraint& other) const noexcept
        {
            return size_constraint{std::max(min, other.min), std::min(max, other.max)};
        }

        const float min;
        const float max;
    };

    constexpr auto free_size = size_constraint{0.f, 1000000.f};

}

#endif