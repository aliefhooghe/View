#ifndef VIEW_ORIENTATION_H_
#define VIEW_ORIENTATION_H_

namespace View {

    /**
     * \enum orientation
     **/
    enum class orientation {
        horizontal,
        vertical
    };

    /**
     * \brief Compute the orthogonal of a given orientation
     */
    constexpr auto orthogonal(const orientation o)
    {
        return o == orientation::horizontal ?
            orientation::vertical : orientation::horizontal;
    }

}

#endif