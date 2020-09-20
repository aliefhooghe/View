#ifndef VIEW_FILESYSTEM_DIRECTORY_MODEL_H_
#define VIEW_FILESYSTEM_DIRECTORY_MODEL_H_

#include <filesystem>

#include "directory_model.h"
#include "alphabetical_compare.h"

namespace View {

    class filesystem_directory_model :
        public abstract_storage_directory_model<std::string, std::string, alphabetical_compare, filesystem_directory_model>
    {
        using item =
            typename directory_model<std::string, std::string, filesystem_directory_model>::item;
        using storage = abstract_storage_directory_model<std::string, std::string, alphabetical_compare, filesystem_directory_model>;

        public:
            filesystem_directory_model() = default;

            filesystem_directory_model(const std::filesystem::path& root)
            : _root{root}
            {}

            void update();

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


        private:
            void _scan();

            bool _scanned{false};
            std::filesystem::path _root{};
    };

}

#endif