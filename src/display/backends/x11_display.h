#ifndef VIEW_X11_DISPLAY_H_
#define VIEW_X11_DISPLAY_H_

#include <thread>
#include <memory>
#include "widget/widget.h"

namespace View {

    /**
     *  \class x11_display
     *  \brief
     */
    class x11_display {

    public:
        x11_display(widget& root, float pixel_per_unit);
        virtual ~x11_display() = default;

    protected:
        void _create_window(Window parent = 0);
        void _wait_window_thread();
        void _close_window();
        bool _is_open() const noexcept  { return _running; }

    private:
        static void _window_proc(x11_display *self, Window parent);

        std::thread _window_thread{};
        bool _running{false};
        widget& _root;
        float _pixel_per_unit;
    };

}

#endif