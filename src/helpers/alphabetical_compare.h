#ifndef VIEW_ALPHABETICAL_COMPARE_H
#define VIEW_ALPHABETICAL_COMPARE_H

#include <string>
#include <locale>

namespace View {

    struct alphabetical_compare {

        bool operator() (const std::string& a, const std::string& b) const noexcept
        {
            return cmp_from_idx(a, b, 0u);
        }

    private:
        //  Compare starting from index
        bool cmp_from_idx(const std::string& a, const std::string& b, std::size_t idx) const noexcept
        {
            if (idx == b.length()) {
                return false;
            }
            else if (idx == a.length()) {
                return true; // a < b
            }
            else {
                auto ac = std::tolower(a[idx]);
                auto bc = std::tolower(b[idx]);

                if (ac == bc) {
                    return cmp_from_idx(a, b, idx + 1);
                }
                else {
                    return ac < bc;
                }
            }
        }
    };

}

#endif //VIEW_ALPHABETICAL_COMPARE_H
