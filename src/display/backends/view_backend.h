#ifndef VIEW_BACKEND_H_
#define VIEW_BACKEND_H_

#include <string>
#include "widget/widget.h"

namespace View
{
    class view_backend {
    public:
        view_backend(widget& root, float pixel_per_unit)
        : _root{root}, _pixel_per_unit{pixel_per_unit}
        {}
        virtual ~view_backend() = default;

        unsigned int px_width() const noexcept { return static_cast<unsigned int>(_root.width() * _pixel_per_unit); }
        unsigned int px_height() const noexcept { return static_cast<unsigned int>(_root.height() * _pixel_per_unit); }

        virtual void create_window(const std::string& title, void *parent = nullptr) = 0;
        virtual void wait_window_thread() = 0;
        virtual void close_window() = 0;
        virtual bool windows_is_open() const noexcept = 0;

        /**
         *  \brief Used to receive keyboard text input from VST2 Host 
         *  \note This should not be implemented if keyboard event can be retrieved by the vst2 plugin
         *  directly from the windows manager
         */
        virtual bool vst2_text_input(const std::string_view) { return false; }
    protected:
        widget& _root;
        const float _pixel_per_unit;
    };


} // namespace View


#endif