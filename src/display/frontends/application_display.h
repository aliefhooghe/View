#ifndef VIEW_APPLICATION_DISPLAY_H_
#define VIEW_APPLICATION_DISPLAY_H_

#include <memory>

#include "display/backends/view_backend.h"

namespace View {

    /**
     *  \class application_display
     *  \brief A classic standalone software windows
     */
    class application_display {
    public:
        application_display(std::unique_ptr<view_backend>&& backend);
        ~application_display() = default;

        /**
         *  \brief Open a windows
         *  \param title windows's title (\TODO type ?)
         */
        void open(const std::string& title);

        /**
         *  \brief Wait for the windows to be closed
         *  \details Return if immediately if the windows is already closed
         */
        void wait();

        /**
         *  \brief Close the windows
         */
        void close();

        /**
         * \brief Return true if the windows is currently open
         */
        bool is_open();

    private:
        std::unique_ptr<view_backend> _backend{};
    };

}

#endif