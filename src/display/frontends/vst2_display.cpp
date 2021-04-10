
#include "vst2_display.h"

namespace View
{
    vst2_display::vst2_display(std::unique_ptr<view_backend>&& backend)
    : _backend{std::move(backend)}
    {}

    void vst2_display::open(void *parent_handle)
    {
        _backend->create_window("VIEW VST2", parent_handle);
    }

    void vst2_display::close()
    {
        _backend->close_window();
    }

    unsigned int vst2_display::px_width()
    {
        return _backend->px_width();
    }

    unsigned int vst2_display::px_height()
    {
        return _backend->px_height();
    }

} // namespace View
