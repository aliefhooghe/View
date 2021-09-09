#ifndef VIEW_WIN_BACKEND_H_
#define VIEW_WIN_BACKEND_H_

#include <thread>
#include <memory>

#include "view_backend.h"

namespace View {

    class win32_window;

    /**
     *  \class win32 backend
     *  \brief
     */
    class win32_backend : public view_backend {

    public:
        win32_backend(widget& root, float pixel_per_unit);
        virtual ~win32_backend() = default;

        void create_window(const std::string& title, void* parent = nullptr) override;
        void wait_window_thread() override;
        void close_window() override;
        bool windows_is_open() const noexcept override;

        bool vst2_char_input(char) override;
    private:
        static void _app_window_proc(win32_backend* self, const std::string& title);

        std::unique_ptr<win32_window> _window{};
        std::thread _window_thread{};
        bool _running{false};
    };

}

#endif