#ifndef VIEW_VST2_DISPLAY_H_
#define VIEW_VST2_DISPLAY_H_

namespace View {

    /**
     *  \class vst2 display
     *  \brief A display suited for integration in a vst2 plugin
     */
    class vst2_display {
    public:
        virtual ~vst2_display() = default;

        /**
         *  \brief Open
         *  \param parent_handle An os specific handle to the parent window
         */
        virtual void open(void *parent_handle) = 0;

        /**
         *  \brief Close the display
         */
        virtual void close() = 0;

        /**
         *
         *  \brief Return width in pixel
         */
        virtual unsigned int px_width() = 0;

        /**
         *
         *  \brief Return height in pixel
         */
        virtual unsigned int px_height() = 0;
    };

}

#endif