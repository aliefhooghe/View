#ifndef VIEW_DIRECTORY_VIEW_H
#define VIEW_DIRECTORY_VIEW_H

#include <vector>
#include <set>
#include <cmath>
#include "helpers/directory_model.h"
#include "control.h"
#include "drawing/text_helper.h"

namespace View {

    template <typename DerivedModel>
    class directory_view;

    template <typename DerivedModel>
    class owning_directory_view;

    template <typename DerivedModel>
    auto make_directory_view(DerivedModel& model, float width, float height, float cell_height = 16.f, float font_size = 14.f)
    {
        return std::make_unique<directory_view<DerivedModel>>(model, width, height, cell_height, font_size);
    }

    template <typename DerivedModel>
    auto make_directory_view(std::unique_ptr<DerivedModel>&& model, float width, float height, float cell_height = 16.f, float font_size = 14.f)
    {
        return std::make_unique<owning_directory_view<DerivedModel>>(std::move(model), width, height, cell_height, font_size);
    }

    // template <typename Key, typename Value, typename Model>

    template <typename DerivedModel>
    class directory_view : public control {

        template <typename k, typename v, typename m>
        static constexpr auto& _type_helper(const directory_model<k, v, m>& impl) { return impl; }

        using model = std::decay_t<decltype(_type_helper(std::declval<DerivedModel>()))>;
        using key = typename model::key;
        using value = typename model::value;
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

        auto caption_by_key(const key& x)
        {
            if constexpr (std::is_convertible_v<key, std::string>)
                return std::string{x};
            else
                return std::to_string(x);
        }

    public:
        using value_select_callback = std::function<void(value&)>;
        using directory_select_callback = std::function<void(DerivedModel&)>;

        directory_view(DerivedModel& m, float width, float height, float cell_height = 16.f, float font_size = 14.f);
        ~directory_view() override;

        void draw(NVGcontext *vg) override;
        // todo : void draw_rect(cairo_t *cr, const rectangle<>& area) override;

        void reset_selection() noexcept;
        bool select_directory(const DerivedModel&);
        bool select_value(const value&);

        virtual void update();
        void close_all_directories();
        void set_value_select_callback(value_select_callback);
        void set_directory_select_callback(directory_select_callback);

        void apply_color_theme(const color_theme&) override;

        bool on_mouse_wheel(float x, float y, float distance) override ;
        bool on_mouse_move(float x, float y) override;

        bool on_mouse_button_up(const mouse_button button, float x, float y) override;

    protected:
        DerivedModel& data_model() noexcept { return _model.self(); }
        const DerivedModel& data_model() const noexcept { return _model.self(); }

    private:
        template <typename TPredicat>
        bool _select_item(const DerivedModel& parent, TPredicat pred)
        {
            for (const auto& pair : parent) {
                const auto item_ref = &(pair.second);
                if (pred(pair.second)) {
                    _selected_item = item_ref;
                    // Open if found item is a directory
                    if (std::holds_alternative<DerivedModel>(pair.second))
                        _open_dirs.insert(item_ref);
                    return true;
                }
                else if (std::holds_alternative<DerivedModel>(pair.second)) {
                    const auto& subdir = std::get<DerivedModel>(pair.second);
                    if (_select_item(subdir, pred)) {
                        _open_dirs.insert(item_ref);
                        return true;
                    }
                }
            }
            return false;
        }

        void unfold();
        bool cell_at(float y, unsigned int &idx);

        //  update helper
        void add_cells(const key&, item&, unsigned int level = 0u);

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

    template <typename DerivedModel>
    class owning_directory_view : public directory_view<DerivedModel>
    {
    public:
        owning_directory_view(std::unique_ptr<DerivedModel>&& model, float width, float height, float cell_height = 16.f, float font_size = 14.f)
        :   directory_view<DerivedModel>{*model, width, height, cell_height, font_size},
            _model{std::move(model)}
        {}

        ~owning_directory_view() override = default;

    private:
        std::unique_ptr<DerivedModel> _model;
    };

    template <typename DerivedModel>
    directory_view<DerivedModel>::directory_view(
        DerivedModel &m,
        float width, float height,
        float cell_height, float font_size)
    :   control{
            width, height,
            size_constraint::at_least(4.f * cell_height),
            size_constraint::at_least(2.f * cell_height)
        },
        _model{m.self()},
        _cell_height{cell_height},
        _font_size{font_size}
    {
        apply_color_theme(default_color_theme);
        unfold();
    }

    template<typename DerivedModel>
    directory_view<DerivedModel>::~directory_view()
    {
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::draw(NVGcontext *vg)
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

    template<typename DerivedModel>
    void directory_view<DerivedModel>::unfold()
    {
        _cells.clear();

        for (auto& node : _model)
            add_cells(node.first, node.second, 0);

        invalidate();
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::reset_selection() noexcept
    {
        _selected_item = nullptr;
        invalidate();
    }

    template <typename DerivedModel>
    bool directory_view<DerivedModel>::select_directory(const DerivedModel& directory)
    {
        const bool selected =
            _select_item(
                _model.self(),
                [&directory](const item& i)
                {
                    if (std::holds_alternative<DerivedModel>(i))
                        return &std::get<DerivedModel>(i) == &directory;
                    else
                        return false;
                });

        if (selected) {
            unfold();
            invalidate();
        }

        return selected;
    }

    template <typename DerivedModel>
    bool directory_view<DerivedModel>::select_value(const value& val)
    {
        const bool selected =
            _select_item(
                _model.self(),
                [&val](const item& i)
                {
                    if (std::holds_alternative<value>(i))
                        return &std::get<value>(i) == &val;
                    else
                        return false;
                });

        if (selected) {
            unfold();
            invalidate();
        }

        return selected;
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::update()
    {
        reset_selection();
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

    template<typename DerivedModel>
    void directory_view<DerivedModel>::close_all_directories()
    {
        reset_selection();
        _open_dirs.clear();
        unfold();
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::set_value_select_callback(value_select_callback callback)
    {
        _value_select_callback = callback;
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::set_directory_select_callback(directory_select_callback callback)
    {
        _directory_select_callback = callback;
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::apply_color_theme(const color_theme &theme)
    {
        _selected_color = theme.secondary;
        _hoverred_color = theme.secondary_light;
        _default_color = theme.on_surface;
    }

    template<typename DerivedModel>
    bool directory_view<DerivedModel>::on_mouse_wheel(float, float, float distance)
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

    template<typename DerivedModel>
    bool directory_view<DerivedModel>::on_mouse_move(float x, float y)
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

    template<typename DerivedModel>
    bool directory_view<DerivedModel>::on_mouse_button_up(const mouse_button button, float x, float y)
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

    template<typename DerivedModel>
    void directory_view<DerivedModel>::add_cells(const key& k, item& i, unsigned int level)
    {
        const auto type =
            std::holds_alternative<value>(i) ? cell_type::value : cell_type::directory;

        _cells.emplace_back(&i, level, caption_by_key(k), type);

        //  if this is an open directory
        if (type == cell_type::directory && _open_dirs.count(&i) != 0) {
            auto& dir = std::get<DerivedModel>(i);

            for (auto& node : dir)
                add_cells(node.first, node.second, level + 1u);
        }
    }

    template<typename DerivedModel>
    void directory_view<DerivedModel>::on_cell_click(const unsigned int idx, float x)
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
                    _selected_item = c.ref;
                    _directory_select_callback(std::get<DerivedModel>(*c.ref));
                    invalidate();
                }
            }

        }
        else {
            if (_selected_item != c.ref) {
                _selected_item = c.ref;
                if (_value_select_callback)
                    _value_select_callback(std::get<value>(*c.ref));
                invalidate();
            }
        }
    }

    template<typename DerivedModel>
    bool directory_view<DerivedModel>::is_open(const cell& c) const
    {
        if (c.type == cell_type::directory) {
            return (_open_dirs.count(c.ref) != 0);
        }
        else {
            return false;
        }
    }

    template<typename DerivedModel>
    bool directory_view<DerivedModel>::cell_at(float y, unsigned int& idx)
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
