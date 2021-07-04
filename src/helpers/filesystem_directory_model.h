#ifndef VIEW_FILESYSTEM_DIRECTORY_MODEL_H_
#define VIEW_FILESYSTEM_DIRECTORY_MODEL_H_

#include <filesystem>

#include "directory_model.h"
#include "alphabetical_compare.h"

namespace View {

    class filesystem_directory_model :
        public abstract_storage_directory_model<std::string, std::filesystem::path, alphabetical_compare, filesystem_directory_model>
    {
        using storage = abstract_storage_directory_model<std::string, std::filesystem::path, alphabetical_compare, filesystem_directory_model>;
    public:
        using item = typename storage::item;
        using iterator = typename storage::iterator;

            filesystem_directory_model() = default;
            filesystem_directory_model(const filesystem_directory_model&) = default;
            filesystem_directory_model(filesystem_directory_model&&) noexcept = default;
            ~filesystem_directory_model() noexcept = default;

            filesystem_directory_model& operator =(const filesystem_directory_model&) = default;
            filesystem_directory_model& operator =(filesystem_directory_model&&) noexcept = default;

            filesystem_directory_model(const std::filesystem::path& root)
            : _root{root}
            {}

            /**
             * \brief sync the with the filesystem
             */
            void sync();

            //  directory model interface
            std::size_t size();
            iterator begin();
            iterator end();
            iterator find(const std::string& key);
            const item& operator[](const std::string& key);
            const std::filesystem::path& path() const noexcept;

        private:
            void _initialize();
            static bool _ignore(const std::filesystem::directory_entry&);

            bool _scanned{false};
            std::filesystem::path _root{};
    };

}

#endif