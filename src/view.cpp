
#include "view.h"

#if defined(__linux__) || defined(__APPLE__)
    #include "display/backends/x11_backend.h"
#elif defined _WIN32
    #include "display/backends/win32_backend.h"
#else
# error "No backend is available on this platform"
#endif

namespace View
{
    static std::unique_ptr<view_backend> create_backend(widget& root, float pixel_per_unit)
    {
#if defined(__linux__) || defined(__APPLE__)
        return std::make_unique<x11_backend>(root, pixel_per_unit);
#elif defined _WIN32
        return std::make_unique<win32_backend>(root, pixel_per_unit);
#endif
    }

    std::unique_ptr<application_display> create_application_display(widget& root, float pixel_per_unit)
    {
        return std::make_unique<application_display>(create_backend(root, pixel_per_unit));
    }

    std::unique_ptr<vst2_display> create_vst2_display(widget& root, float pixel_per_unit)
    {
        return std::make_unique<vst2_display>(create_backend(root, pixel_per_unit));
    }
}