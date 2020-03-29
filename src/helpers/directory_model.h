#ifndef VIEW_DIRECTORY_MODEL_H
#define VIEW_DIRECTORY_MODEL_H

#include <variant>
#include <memory>
#include <map>

#include <iostream>

namespace View {
    /**
     * \brief directory_model is a generic trait for n-arry tree
     */
    template <typename Key, typename Value, typename Derived>
    class directory_model {
    public:
        using item =
            std::variant<
                Derived,  /** = sub directory **/
                Value>;

        using node =
            std::pair<const Key, item>;

        auto size() const { return self().size(); }

        auto begin() { return self().begin(); }
        auto begin() const { return self().begin(); }

        auto end() { return self().end(); }
        auto end() const { return self().end(); }

        auto find(const Key& k) { return self().find(k); }
        auto find(const Key& k) const { return self().find(k); }

        item& operator[](const Key& k) { return self()[k]; }
        const item& operator[](const Key& k) const { return self()[k]; }

        Derived& add_directory(const Key& k) { return self().add_directory(k); }
        Derived& add_directory(const Key& k, Derived&& dir) { return self().add_directory(k, std::move(dir)); }

        void add_value(const Key& k, Value&& v) { return self().add_value(k, std::move(v)); }

        bool is_directory(const Key& k) {   return std::holds_alternative<Derived>(self()[k]); }
        bool is_value(const Key& k) { return !is_directory(k); }

    private:
        auto& self()
        {
            return *static_cast<Derived*>(this);
        }

        const auto& self() const
        {
            return *static_cast<const Derived*>(this);
        }
    };

    template <typename Key, typename Value, typename Compare, typename Derived>
    class abstract_storage_directory_model : public directory_model<Key, Value, Derived> {

    public:
        using item = typename directory_model<Key, Value, Derived>::item;

        auto size() const { return _childrens.size(); }

        auto begin() { return _childrens.begin(); }
        auto begin() const { return _childrens.begin(); }

        auto end() { return _childrens.end(); }
        auto end() const { return _childrens.end(); }

        auto find(const Key& k) { return _childrens.find(k); }
        auto find(const Key& k) const { return _childrens.find(k); }

        item& operator[](const Key& k) { return _childrens[k]; }
        const item& operator[](const Key& k) const { return _childrens[k]; }

        Derived& add_directory(const Key& k)
        {
            auto it = _childrens.find(k);

            if (it == _childrens.end()) {
                //  emplace can't fail since k is not in the map
                auto new_it = _childrens.emplace(k, Derived{}).first;
                return std::get<Derived>(new_it->second);
            }
            else {
                if (std::holds_alternative<Derived>(it->second)) {
                    //  return existing directory
                    return std::get<Derived>(it->second);
                }
                else {
                    //  a value is stored with this key, can't create directory
                    throw std::runtime_error("storage_directory_model::add_directory cant creat a directory on an existing value");
                }
            }
        }

        Derived& add_directory(const Key& k, Derived&& dir)
        {
            auto& new_item = _childrens[k];
            new_item = std::move(dir);
            return std::get<Derived>(new_item);
        }

        void add_value(const Key& k, Value&& v)
        {
            _childrens[k] = std::move(v);
        }

        void clear()
        {
            _childrens.clear();
        }

    private:
        std::map<Key, item, Compare> _childrens{};
    };


    template <typename Key, typename Value, typename Compare = std::less<Key>>
    class storage_directory_model : public abstract_storage_directory_model<Key, Value, Compare, storage_directory_model<Key, Value, Compare>> {
    };

}

#endif //VIEW_DIRECTORY_MODEL_H
