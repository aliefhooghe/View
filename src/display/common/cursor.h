#ifndef CURSOR_H_
#define CURSOR_H_

#include "widget/orientation.h"
namespace View {
    /**
     *  \brief PLatform independent cursors identifiers
     *  \details Theses are mapped to closer platform cursors
     **/
    enum class cursor {
        standard,
        arrow,
        cross,
        noway,
        wait,
        hand,
        size_horizontal,
        size_vertical,
        move,
        text,

        _count
    };

    constexpr auto VIEW_CURSOR_COUNT = static_cast<unsigned int>(cursor::_count);

    constexpr cursor make_size_cursor(orientation o)
    {
        return o == orientation::horizontal ? cursor::size_horizontal : cursor::size_vertical;
    }
}


#endif