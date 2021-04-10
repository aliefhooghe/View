#ifndef VIEW_VST2_DISPLAY_H_
#define VIEW_VST2_DISPLAY_H_

#include <memory>

#include "display/backends/view_backend.h"

namespace View {

    /**
     *  \class vst2 display
     *  \brief A display suited for integration in a vst2 plugin
     */
    class vst2_display {
    public:
        vst2_display(std::unique_ptr<view_backend>&& backend);
        ~vst2_display() = default;

        /**
         *  \brief Open
         *  \param parent_handle An os specific handle to the parent window
         */
        void open(void *parent_handle);

        /**
         *  \brief Close the display
         */
        void close();

        /**
         *
         *  \brief Return width in pixel
         */
        unsigned int px_width();

        /**
         *
         *  \brief Return height in pixel
         */
        unsigned int px_height();

    private:
        std::unique_ptr<view_backend> _backend{};
    };

}

#endif