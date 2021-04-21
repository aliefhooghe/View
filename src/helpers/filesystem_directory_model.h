#ifndef VIEW_FILESYSTEM_DIRECTORY_MODEL_H_
#define VIEW_FILESYSTEM_DIRECTORY_MODEL_H_

#include <filesystem>

#include "directory_model.h"
#include "alphabetical_compare.h"

namespace View {

    class filesystem_directory_model :
        public abstract_storage_directory_model<std::string, std::string, alphabetical_compare, filesystem_directory_model>
    {
        using storage = abstract_storage_directory_model<std::string, std::string, alphabetical_compare, filesystem_directory_model>;
        using item = typename storage::item;
        using iterator = typename storage::iterator;

        public:
            filesystem_directory_model() = default;

            filesystem_directory_model(const std::filesystem::path& root)
            : _root{root}
            {}

            /**
             * \brief sync the with the filesystem
             */
            void sync();

            //  directory model interface
            auto size()
            {
                _scan();
                return storage::size();
            }

            auto begin()
            {
                _scan();
                return storage::begin();
            }

            auto end()
            {
                _scan();
                return storage::end();
            }

            auto find(const std::string& key)
            {
                _scan();
                return storage::find(key);
            }

            const auto& operator[](const std::string& key)
            {
                _scan();
                return storage::operator[](key);
            }

            const auto& path() const noexcept { return _root; }

        private:
            void _scan();

            bool _scanned{false};
            std::filesystem::path _root{};
    };

}

#endif