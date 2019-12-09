#ifndef VIEW_APPLICATION_DISPLAY_H_
#define VIEW_APPLICATION_DISPLAY_H_

#include <string>

namespace View {

    /**
     *  \class application_display
     *  \brief A classic standalone software windows
     */
    class application_display {
    public:
        virtual ~application_display() = default;

        /**
         *  \brief Open a windows
         *  \param title windows's title (\TODO type ?)
         */
        virtual void open(const std::string& title) = 0;

        /**
         *  \brief Wait for the windows to be closed
         *  \details Return if imediately if the windows is already closed
         */
        virtual void wait() = 0;

        /**
         *  \brief Close the windows
         */
        virtual void close() = 0;

        /**
         * \brief Return true if the windows is currently open
         */
        virtual bool is_open() = 0;
    };

}

#endif