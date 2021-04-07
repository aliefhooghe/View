#ifndef VIEW_DIRECTORY_VIEW_H
#define VIEW_DIRECTORY_VIEW_H

#include <vector>
#include <set>
#include <cmath>
#include "helpers/directory_model.h"
#include "control.h"
#include "drawing/text_helper.h"

namespace View {

    template <typename Key, typename Value, typename Model>
    class directory_view;

    template <typename Key, typename Value, typename Model>
    class owning_directory_view;

    template <typename Key, typename Value, typename Model>
    auto make_directory_view(directory_model<Key, Value, Model>& model, float width, float height, float cell_height = 16.f, float font_size = 14.f)
    {
        return std::make_unique<directory_view<Key, Value, Model>>(model, width, height, cell_height, font_size);
    }

    namespace details {

        template <typename Key, typename Value, typename Model>
        constexpr auto& directory_model_type_helper(directory_model<Key, Value, Model>& impl) { return impl; }

        template <typename T>
        struct owning_directory_view_type;

        template <typename Key, typename Value, typename Model>
        struct owning_directory_view_type<directory_model<Key, Value, Model>> {
            using type = owning_directory_view<Key, Value, Model>;
        };
    }

    template <typename T>
    auto make_directory_view(std::unique_ptr<T>&& model, float width, float height, float cell_height = 16.f, float font_size = 14.f)
    {
        using model_type = std::decay_t<decltype(details::directory_model_type_helper(*model))>;
        return std::make_unique<typename details::owning_directory_view_type<model_type>::type>(std::move(model), width, height, cell_height, font_size);
    }

    template <typename Key, typename Value, typename Model>
    class directory_view : public control {

        using model = directory_model<Key, Value, Model>;
        using item = typename model::item;
        using node = typename model::node;

        enum class cell_type {value, directory};

        struct cell {

            cell(item *i, const unsigned int l, std::string&& c, const cell_type t)
            :   ref{i}, level{l}, caption{std::move(c)}, type{t}
            {}

            const cell_type type;
            const unsigned int level;
            const std::string caption;
            item* const ref;
        };

        auto caption_by_key(const Key& x)
        {
            if constexpr (std::is_convertible_v<Key, std::string>)
                return std::string{x};
            else
                return std::to_string(x);
        }

    public:
        using value_select_callback = std::function<void(const Value&)>;
        using directory_select_callback = std::function<void(Model&)>;

        directory_view(model& m, float width, float height, float cell_height, float font_size);
        ~directory_view() override;

        void draw(NVGcontext *vg) override;
        // todo : void draw_rect(cairo_t *cr, const rectangle<>& area) override;

        void update();
        void close_all_directories();
        void set_value_select_callback(value_select_callback);
        void set_directory_select_callback(directory_select_callback);

        void apply_color_theme(const color_theme&) override;

        bool on_mouse_wheel(float x, float y, float distance) override ;
        bool on_mouse_move(float x, float y) override;

        bool on_mouse_button_up(const mouse_button button, float x, float y) override;

        Model& data_model() noexcept { return _model.self(); }
        const Model& data_model() const noexcept { return _model.self(); }

    private:
        void unfold();
        bool cell_at(float y, unsigned int &idx);

        //  update helper
        void add_cells(const Key&, item&, unsigned int level = 0u);

        void on_cell_click(const unsigned int idx, float x);
        bool is_open(const cell& c) const;

        //  geometry helper
        auto cell_width_offset(const cell& c) { return c.level * _cell_height * 0.5f;  } 

        std::set<const item*> _open_dirs{};

        //  model ref
        model& _model;

        //  View
        std::vector<cell> _cells{};
        unsigned int _display_cell_begin{0u};
        int _hoverred_cell{-1};
        const item* _selected_item{nullptr};
        value_select_callback _value_select_callback{nullptr};
        directory_select_callback _directory_select_callback{nullptr};

        const float _cell_height;
        const float _font_size;

        NVGcolor _selected_color;
        NVGcolor _hoverred_color;
        NVGcolor _default_color;
    };

    template <typename Key, typename Value, typename Model>
    class owning_directory_view : public directory_view<Key, Value, Model>
    {
        using model = directory_model<Key, Value, Model>;
    public:
        owning_directory_view(std::unique_ptr<model>&& model, float width, float height, float cell_height, float font_size)
        :   directory_view<Key, Value, Model>{*model, width, height, cell_height, font_size},
            _model{std::move(model)}
        {}

    private:
        std::unique_ptr<model> _model;
    };

    template<typename Key, typename Value, typename Model>
    directory_view<Key, Value, Model>::directory_view(
        directory_view::model &m,
        float width, float height,
        float cell_height, float font_size)
    :   control{
            width, height,
            size_constraint{5, std::numeric_limits<float>::infinity()},
            size_constraint{1, std::numeric_limits<float>::infinity()}
        },
        _model{m},
        _cell_height{cell_height},
        _font_size{font_size}
    {
        apply_color_theme(default_color_theme);
        unfold();
    }

    template<typename Key, typename Value, typename Model>
    directory_view<Key, Value, Model>::~directory_view()
    {
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::draw(NVGcontext *vg)
    {
        //  The max number of cell that can fit in widget rectangle
        const auto max_display_cell_count =
            static_cast<std::size_t>(std::floor(height()/_cell_height));

        //  The number actually available
        const auto display_cell_count =
            std::min(max_display_cell_count, _cells.size() - _display_cell_begin);

        for (auto i = 0u; i < display_cell_count; ++i) {
            const auto idx = _display_cell_begin + i;
            const cell& c = _cells[idx];

            //  Draw Cell Background
            const auto width_offset = cell_width_offset(c);
            const auto height_offset = static_cast<float>(i) * _cell_height;
            const auto content_color =
                    _selected_item == c.ref ? _selected_color : (
                        hovered() && idx == _hoverred_cell ? _hoverred_color : _default_color);

            nvgFillColor(vg, content_color);

            //  Arrow
            if (c.type == cell_type::directory) {
                const auto arrow_offset = 0.3f * _cell_height;
                
                nvgBeginPath(vg);
                nvgMoveTo(vg, width_offset + arrow_offset, height_offset + arrow_offset);

                if (is_open(c)) {
                    nvgLineTo(vg, width_offset + _cell_height - arrow_offset, height_offset + arrow_offset);
                    nvgLineTo(vg, width_offset + _cell_height / 2.f, height_offset + _cell_height - arrow_offset);
                }
                else {
                    nvgLineTo(vg, width_offset + arrow_offset, height_offset + _cell_height - arrow_offset);
                    nvgLineTo(vg, width_offset + _cell_height - arrow_offset, height_offset + _cell_height / 2.f);
                }
                
                nvgFill(vg);
            }

            //  Text
            draw_text(
                vg, width_offset + _cell_height, height_offset, width(), _cell_height, _font_size, c.caption.c_str(), c.type == cell_type::directory,
                horizontal_alignment::left, vertical_alignment::bottom);
        }
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::unfold()
    {
        _cells.clear();

        for (auto& node : _model)
            add_cells(node.first, node.second, 0);

        invalidate();
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::update()
    {
        _selected_item = nullptr;
        unfold();

        // Clean open dir set : keep only item that still exist
        std::set<const item*> new_open_dirs{};
        for (auto& c : _cells) {
            if (is_open(c)) {
                new_open_dirs.insert(c.ref);
            }
        }
        _open_dirs = std::move(new_open_dirs);
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::close_all_directories()
    {
        _open_dirs.clear();
        unfold();
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::set_value_select_callback(value_select_callback callback)
    {
        _value_select_callback = callback;
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::set_directory_select_callback(directory_select_callback callback)
    {
        _directory_select_callback = callback;
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::apply_color_theme(const color_theme &theme)
    {
        _selected_color = theme.secondary;
        _hoverred_color = theme.secondary_light;
        _default_color = theme.on_surface;
    }

    template<typename Key, typename Value, typename Model>
    bool directory_view<Key, Value, Model>::on_mouse_wheel(float, float, float distance)
    {
        if (_cells.size() > 0) {
            _display_cell_begin = std::clamp(
                static_cast<int>(_display_cell_begin) - static_cast<int>(distance),
                0, static_cast<int>(_cells.size() - 1));
            invalidate();
            return true;
        }
        else {
            return false;
        }
    }

    template<typename Key, typename Value, typename Model>
    bool directory_view<Key, Value, Model>::on_mouse_move(float x, float y)
    {
        unsigned new_hoverred_idx;

        if (!cell_at(y, new_hoverred_idx)) {
            if (_hoverred_cell != -1) {
                _hoverred_cell = -1;
                invalidate();
            }
        }
        else if (_hoverred_cell != new_hoverred_idx) {
            _hoverred_cell = new_hoverred_idx;
            invalidate();
        }

        return true;
    }

    template<typename Key, typename Value, typename Model>
    bool directory_view<Key, Value, Model>::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        unsigned int idx;
        if (button == mouse_button::left && cell_at(y, idx)) {
            on_cell_click(idx, x);
            return true;
        }
        else {
            return false;
        }
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::add_cells(const Key& k, item& i, unsigned int level)
    {
        const auto type =
            std::holds_alternative<Value>(i) ? cell_type::value : cell_type::directory;

        _cells.emplace_back(&i, level, caption_by_key(k), type);

        //  if this is an open directory
        if (type == cell_type::directory && _open_dirs.count(&i) != 0) {
            auto& dir = std::get<Model>(i);

            for (auto& node : dir)
                add_cells(node.first, node.second, level + 1u);
        }
    }

    template<typename Key, typename Value, typename Model>
    void directory_view<Key, Value, Model>::on_cell_click(const unsigned int idx, float x)
    {
        auto& c = _cells[idx];

        if (c.type == cell_type::directory) {

            if (!_directory_select_callback || (x < cell_width_offset(c) + _cell_height)) {    
                //  Swap open state
                if (is_open(c))
                    _open_dirs.erase(c.ref);
                else
                    _open_dirs.insert(c.ref);

                unfold();
            }
            else {
                if (_selected_item != c.ref) {
                    if (_directory_select_callback)
                        _directory_select_callback(std::get<Model>(*c.ref));
                }
            }

        }
        else {
            if (_selected_item != c.ref) {
                if (_value_select_callback)
                    _value_select_callback(std::get<Value>(*c.ref));
                invalidate();
            }
        }

        _selected_item = c.ref;
    }

    template<typename Key, typename Value, typename Model>
    bool directory_view<Key, Value, Model>::is_open(const cell& c) const
    {
        if (c.type == cell_type::directory) {
            return (_open_dirs.count(c.ref) != 0);
        }
        else {
            return false;
        }
    }

    template<typename Key, typename Value, typename Model>
    bool directory_view<Key, Value, Model>::cell_at(float y, unsigned int& idx)
    {
        const auto i =
            static_cast<unsigned int>(y / _cell_height) + _display_cell_begin;

        if (i >= _cells.size()) {
            return false;
        }
        else {
            idx = i;
            return true;
        }
    }

}


#endif //VIEW_DIRECTORY_VIEW_H
