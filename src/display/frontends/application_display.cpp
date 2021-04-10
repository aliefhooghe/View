
#include "application_display.h"

namespace View {

    application_display::application_display(std::unique_ptr<view_backend>&& backend)
    : _backend{std::move(backend)}
    {
    }

    void application_display::open(const std::string& title)
    {
        _backend->create_window(title);
    }

    void application_display::wait()
    {
        _backend->wait_window_thread();
    }

    void application_display::close()
    {
        _backend->close_window();
    }

    bool application_display::is_open()
    {
        return _backend->windows_is_open();
    }

} /* View */