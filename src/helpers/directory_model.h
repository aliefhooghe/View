#ifndef VIEW_DIRECTORY_MODEL_H
#define VIEW_DIRECTORY_MODEL_H

#include <variant>
#include <memory>
#include <map>
#include <functional>
#include <iostream>

namespace View {
    /**
     * \brief directory_model is a generic trait for n-arry tree
     */
    template <typename Key, typename Value, typename Derived>
    class directory_model {
    public:
        //  Item default initialization is Derived
        using key = Key;
        using value = Value;
        using item =
            std::variant<
                Derived,  /** = sub directory **/
                Value>;

        using node =
            std::pair<const Key, item>;

        directory_model() noexcept = default;
        directory_model(const directory_model&) = default;
        directory_model(directory_model&&) noexcept = default;
        ~directory_model() noexcept = default;

        directory_model& operator= (const directory_model&) = default;
        directory_model& operator= (directory_model&&) noexcept = default;

        auto size() const { return self().size(); }

        auto begin() { return self().begin(); }
        auto begin() const { return self().begin(); }

        auto end() { return self().end(); }
        auto end() const { return self().end(); }

        auto find(const Key& k) { return self().find(k); }
        auto find(const Key& k) const { return self().find(k); }

        item& operator[](const Key& k) { return self()[k]; }
        const item& operator[](const Key& k) const { return self()[k]; }


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
        using iterator = typename std::map<Key, item, Compare>::iterator;

        abstract_storage_directory_model() noexcept = default;
        abstract_storage_directory_model(const abstract_storage_directory_model&) = default;
        abstract_storage_directory_model(abstract_storage_directory_model&&) noexcept = default;
        ~abstract_storage_directory_model() noexcept = default;

        abstract_storage_directory_model& operator= (const abstract_storage_directory_model&) = default;
        abstract_storage_directory_model& operator= (abstract_storage_directory_model&&) noexcept = default;

        auto size() const { return _childrens.size(); }

        auto begin() { return _childrens.begin(); }
        auto begin() const { return _childrens.begin(); }

        auto end() { return _childrens.end(); }
        auto end() const { return _childrens.end(); }

        auto find(const Key& k) { return _childrens.find(k); }
        auto find(const Key& k) const { return _childrens.find(k); }

        item& operator[](const Key& k) { return _childrens[k]; }
        const item& operator[](const Key& k) const { return _childrens[k]; }

    protected:
        Derived& get_or_create_directory(const Key& k)
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

        Derived& insert_directory(const Key& k, Derived&& dir)
        {
            auto& new_item = _childrens[k];
            new_item = std::move(dir);
            return std::get<Derived>(new_item);
        }

        Value& insert_value(const Key& k, Value&& v)
        {
            auto& new_item = _childrens[k];
            new_item = std::move(v);
            return std::get<Value>(new_item);
        }

        void clear()
        {
            _childrens.clear();
        }

        void erase(const Key& k)
        {
            _childrens.erase(k);
        }

        void erase(iterator it)
        {
            _childrens.erase(it);
        }

        item& move(const Key& k1, const Key& k2)
        {
            if (_childrens.find(k2) != _childrens.end())
                throw std::invalid_argument("move : can't overwrite target");

            auto node = _childrens.extract(k1);

            if (!node.empty()) {
                node.key() = k2;
                _childrens.insert(std::move(node));
                return operator[](k2);
            }
            else {
                throw std::invalid_argument("move : Unknown key");
            }
        }

    private:
        std::map<Key, item, Compare> _childrens{};
    };


    template <typename Key, typename Value, typename Compare = std::less<Key>>
    class storage_directory_model : public abstract_storage_directory_model<Key, Value, Compare, storage_directory_model<Key, Value, Compare>> {
        using implem = abstract_storage_directory_model<Key, Value, Compare, storage_directory_model<Key, Value, Compare>>;
    public:
        using item = typename implem::item;
        using iterator = typename implem::iterator;

        using implem::move;
        using implem::get_or_create_directory;
        using implem::insert_directory;
        using implem::insert_value;
        using implem::clear;
        using implem::erase;
    };

}

#endif //VIEW_DIRECTORY_MODEL_H
