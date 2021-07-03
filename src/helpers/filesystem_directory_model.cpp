
#include "filesystem_directory_model.h"

namespace View {

    void filesystem_directory_model::sync()
    {
        //  Lazy evaluation, we will scan when needed
        if (!_scanned)
            return;

        //  Remove files and directory that does not anymore exist
        for (auto it = begin(); it != end();) {
            const auto current_it = it++;

            if (std::holds_alternative<filesystem_directory_model>(current_it->second)) {
                auto& subdir = std::get<filesystem_directory_model>(current_it->second);

                if (std::filesystem::is_directory(subdir.path()))
                    subdir.sync();
                else
                    erase(current_it);
            }
            else { // file
                const auto& file_path = std::get<std::filesystem::path>(current_it->second);

                if (!std::filesystem::is_regular_file(file_path))
                    erase(current_it);
            }
        }

        //  Add new content
        try {
            for (const auto& entry : std::filesystem::directory_iterator(_root)) {
                if (_ignore(entry))
                    continue;

                const std::string key{entry.path().filename().string()};
                if (find(key) == end()) {
                    if (entry.is_directory())
                        insert_directory(key, filesystem_directory_model{entry});
                    else
                        insert_value(key, std::filesystem::path{entry.path()});
                }
            }
        }
        catch(std::filesystem::filesystem_error&)
        {
            //  this is not a fatal error
        }
    }

    std::size_t filesystem_directory_model::size()
    {
        _initialize();
        return storage::size();
    }

    filesystem_directory_model::iterator filesystem_directory_model::begin()
    {
        _initialize();
        return storage::begin();
    }

    filesystem_directory_model::iterator filesystem_directory_model::end()
    {
        _initialize();
        return storage::end();
    }

    filesystem_directory_model::iterator filesystem_directory_model::find(const std::string &key)
    {
        _initialize();
        return storage::find(key);
    }

    const filesystem_directory_model::item &filesystem_directory_model::operator[](const std::string &key)
    {
        _initialize();
        return storage::operator[](key);
    }

    const std::filesystem::path &filesystem_directory_model::path() const noexcept
    {
        return _root;
    }

    void filesystem_directory_model::_initialize()
    {
        if (_scanned)
            return;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(_root)) {
                if (_ignore(entry))
                    continue;

                const std::string key{entry.path().filename().string()};
                if (entry.is_directory())
                    insert_directory(key, filesystem_directory_model{entry});
                else
                    insert_value(key, std::filesystem::path{entry.path()});
            }
        }
        catch(std::filesystem::filesystem_error&)
        {
            //  this is not a fatal error
        }

        _scanned = true;
    }

    bool filesystem_directory_model::_ignore(const std::filesystem::directory_entry& entry)
    {
        if (entry.is_directory() || entry.is_regular_file()) {
            const auto filename = entry.path().filename().string();
            return filename == "." || filename == "..";
        }
        else {
            return true;
        }
    }
}