
#include "filesystem_directory_model.h"

namespace View {

    void filesystem_directory_model::sync()
    {
        clear();
        _scanned = false;
        _scan();
    }

    void filesystem_directory_model::_scan()
    {
        if (_scanned)
            return;

        try {
            for (const auto& p : std::filesystem::directory_iterator(_root)) {
                const std::string entry{p.path().filename().string()};

                /**  \todo : better way to handle hidden files **/
                if (entry[0] == '.')
                    continue;

                if (std::filesystem::is_directory(p))
                    add_directory(entry, filesystem_directory_model{p});
                else
                    add_value(entry, p.path().string());
            }
        }
        catch(std::filesystem::filesystem_error&)
        {
            //  this is not a fatal error
            /** \todo log something ! **/
        }

        _scanned = true;
    }


}