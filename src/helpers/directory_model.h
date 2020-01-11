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

        auto size() const { return self().size(); }

        auto begin() { return self().begin(); }
        auto begin() const { return self().begin(); }

        auto end() { return self().end(); }
        auto end() const { return self().end(); }

        auto find() { return self().find(); }
        auto find() const { return self().find(); }

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

    template <typename Key, typename Value, typename Compare = std::less<Key>>
    class storage_directory_model : public directory_model<Key, Value, storage_directory_model<Key, Value, Compare>> {

    public:
        using item = typename directory_model<Key, Value, storage_directory_model>::item;

        auto size() const { return _childrens.size(); }

        auto begin() { return _childrens.begin(); }
        auto begin() const { return _childrens.begin(); }

        auto end() { return _childrens.end(); }
        auto end() const { return _childrens.end(); }

        auto find() { return _childrens.find(); }
        auto find() const { return _childrens.find(); }

        item& operator[](const Key& k) { return _childrens[k]; }
        const item& operator[](const Key& k) const { return _childrens[k]; }

        storage_directory_model& add_directory(const Key& k)
        {
            auto& new_item = _childrens[k];
            new_item = storage_directory_model{};
            return std::get<storage_directory_model>(new_item);
        }

        storage_directory_model& add_directory(const Key& k, storage_directory_model&& dir)
        {
            auto& new_item = _childrens[k];
            new_item = std::move(dir);
            return std::get<storage_directory_model>(new_item);
        }

        void add_value(const Key& k, Value&& v)
        {
            _childrens[k] = v;
        }

    private:
        std::map<Key, item> _childrens{};
    };


}

#endif //VIEW_DIRECTORY_MODEL_H
