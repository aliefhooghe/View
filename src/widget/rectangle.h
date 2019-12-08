#ifndef VIEW_RECTANGLE_H_
#define VIEW_RECTANGLE_H_

#include <algorithm>
#include <ostream>

namespace View {

    template <typename T = float>
    struct rectangle {
        constexpr rectangle() = default;

        constexpr rectangle(T t, T b, T l, T r)
        : top{t}, bottom{b}, left{l}, right{r}
        {}

        template <typename T2>
        constexpr rectangle(const rectangle<T2>& rect)
        :   rectangle{
                static_cast<T>(rect.top),
                static_cast<T>(rect.bottom),
                static_cast<T>(rect.left),
                static_cast<T>(rect.right)}
        {}

        template <typename T2>
        constexpr auto operator =(const rectangle<T2>& rect)
        {
            top = static_cast<T>(rect.top);
            bottom = static_cast<T>(rect.bottom);
            left = static_cast<T>(rect.left);
            right = static_cast<T>(rect.right);
        }

        constexpr rectangle(const rectangle&) = default;

        //  Size
        constexpr auto width() const    { return right - left; }
        constexpr auto height() const   { return bottom - top; }

        //  Operations
        constexpr auto translate(float dx, float dy) const
        {
            return rectangle{top + dy, bottom + dy, left + dx, right + dx};
        }

        constexpr bool overlap(const rectangle& rect) const
        {
            const auto intersect_top = std::max(top, rect.top);
            const auto intersect_bottom = std::min(bottom, rect.bottom);

            const auto intersect_left = std::max(left, rect.left);
            const auto intersect_right = std::min(right, rect.right);

            return (intersect_top < intersect_bottom &&
                    intersect_left < intersect_right);
        }

        constexpr bool intersect(const rectangle& rect, rectangle& intersection) const
        {
            const auto intersect_top = std::max(top, rect.top);
            const auto intersect_bottom = std::min(bottom, rect.bottom);

            const auto intersect_left = std::max(left, rect.left);
            const auto intersect_right = std::min(right, rect.right);

            if (intersect_top < intersect_bottom &&
                intersect_left < intersect_right) {
                intersection = rectangle{intersect_top, intersect_bottom, intersect_left, intersect_right};
                return true;
            }
            else {
                return false;
            }
        }

        constexpr auto bounding(const rectangle& rect) const
        {
            const auto bounding_top = std::min(top, rect.top);
            const auto bounding_bottom = std::max(bottom, rect.bottom);

            const auto bounding_left = std::min(left, rect.left);
            const auto bounding_right = std::max(right, rect.right);

            return rectangle{
                bounding_top,
                bounding_bottom,
                bounding_left,
                bounding_right};
        }

        T top{}, bottom{}, left{}, right{};
    };

    template <typename T>
    rectangle(T,T,T,T) -> rectangle<T>;

    template <typename T1, typename T2, typename T3, typename T4>
    constexpr auto make_rectangle(const T1 top, const T2 bottom, const T3 left, const T4 right)
    {
        return rectangle<T1>{
            top,
            static_cast<T1>(bottom),
            static_cast<T1>(left),
            static_cast<T1>(right)};
    }

    template <typename T>
    auto& operator <<(std::ostream& stream, const rectangle<T>& rect)
    {
        return (
            stream  << "[top : " << rect.top
                    << ", bottom : " << rect.bottom
                    << ", left : " << rect.left
                    << ", right : " << rect.right << "]");
    }
}

#endif