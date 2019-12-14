#ifndef CURSOR_H_
#define CURSOR_H_

namespace View {
    /**
     *  \brief PLatform independant cursors identifiers
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

        _count
    };

    constexpr auto VIEW_CURSOR_COUNT = static_cast<unsigned int>(cursor::_count);
}

#endif