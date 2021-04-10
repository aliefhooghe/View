#ifndef VIEW_X11_BACKEND_H_
#define VIEW_X11_BACKEND_H_

#include <thread>
#include <memory>

#include "view_backend.h"

namespace View {

    /**
     *  \class x11_backend
     *  \brief
     */
    class x11_backend : public view_backend {

    public:
        x11_backend(widget& root, float pixel_per_unit);
        virtual ~x11_backend() = default;

        void create_window(const std::string& title, void *parent = nullptr) override;
        void wait_window_thread() override;
        void close_window() override;
        bool windows_is_open() const noexcept override;

    private:
        typedef unsigned long Window;
        static void _window_proc(x11_backend *self, Window parent);

        std::thread _window_thread{};
        bool _running{false};
    };

}

#endif