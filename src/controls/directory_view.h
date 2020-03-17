#ifndef VIEW_DIRECTORY_VIEW_H
#define VIEW_DIRECTORY_VIEW_H

#include <vector>
#include <set>
#include <cmath>

#include "helpers/directory_model.h"
#include "control.h"
#include "drawing/text_helper.h"

namespace View {

    template <typename Key, typename Value, typename Derived>
    class directory_view;

    template <typename Key, typename Value, typename Derived>
    auto make_directory_view(directory_model<Key, Value, Derived>& model, float width, float height, float cell_height = 3, float font_size = 2.5)
    {
        return std::make_unique<directory_view<Key, Value, Derived>>(model, width, height, cell_height, font_size);
    }

    template <typename Key, typename Value, typename Derived>
    class directory_view : public control {

        using model = directory_model<Key, Value, Derived>;
        using item = typename model::item;
        using node = typename model::node;

        enum class cell_type {value, directory};

        struct cell {
            cell(const model *m, const unsigned int l, std::string&& c, const cell_type t)
            :   dir{m}, level{l}, caption{std::move(c)}, type{t}
            {}

            const model * const dir;
            const unsigned int level;
            const std::string caption;
            const cell_type type;
        };

        auto caption_by_key(const Key& x)
        {
            if constexpr (std::is_convertible_v<Key, std::string>)
                return std::string{x};
            else
                return std::to_string(x);
        }

    public:
        directory_view(model& m, float width, float height, float cell_height, float font_size);
        ~directory_view() override = default;

        void draw(cairo_t *cr) override;
        // todo : void draw_rect(cairo_t *cr, const rectangle<>& area) override;

        void update();
        void apply_color_theme(const color_theme&) override;

        bool on_mouse_wheel(float distance) override ;
        bool on_mouse_move(float x, float y) override;

        bool on_mouse_button_up(const mouse_button button, float x, float y) override;
        bool on_mouse_drag_end(const mouse_button button, float x, float y) override;

    private:
        bool cell_at(float y, unsigned int &idx);

        //  update helpers
        void add_cells(const Key&, model&, unsigned int level = 0);
        void add_cells(const Key&, const Value&, unsigned int level = 0);
        void on_cell_click(const unsigned int idx);

        bool is_open(const cell& c) { return _open_dirs.count(c.dir) != 0; }

        std::set<const model*> _open_dirs{};

        //  model ref
        model& _model;

        //  View
        std::vector<cell> _cells{};
        unsigned int _display_cell_begin{0u};
        unsigned int _hoverred_cell{0u};

        const float _cell_height;
        const float _font_size;

        color _selected_color;
        color _hoverred_color;
        color _default_color;
    };

    template<typename Key, typename Value, typename Derived>
    directory_view<Key, Value, Derived>::directory_view(
        directory_view::model &m,
        float width, float height,
        float cell_height, float font_size)
    :   control{width, height},
        _model{m},
        _cell_height{cell_height},
        _font_size{font_size}
    {
        apply_color_theme(default_color_theme);
        update();
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::draw(cairo_t *cr)
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
            const auto width_offset = c.level * _cell_height;
            const auto height_offset = static_cast<float>(i) * _cell_height;
            const color content_color =
                (hovered() && idx == _hoverred_cell) ? _hoverred_color : _default_color;

            set_source(cr, content_color);

            //  Arrow
            if (c.type == cell_type::directory) {
                const auto arrow_offset = 0.3f * _cell_height;
                cairo_new_sub_path(cr);
                cairo_move_to(cr, width_offset + arrow_offset, height_offset + arrow_offset);

                if (is_open(c)) {
                    cairo_line_to(cr, width_offset + _cell_height - arrow_offset, height_offset + arrow_offset);
                    cairo_line_to(cr, width_offset + _cell_height / 2.f, height_offset + _cell_height - arrow_offset);
                }
                else {
                    cairo_line_to(cr, width_offset + arrow_offset, height_offset + _cell_height - arrow_offset);
                    cairo_line_to(cr, width_offset + _cell_height - arrow_offset, height_offset + _cell_height / 2.f);
                }

                cairo_close_path(cr);
                cairo_fill(cr);
            }

            //  Text
            draw_text(
                cr, width_offset + _cell_height, height_offset, width(), _cell_height, _font_size, c.caption.c_str(), c.type == cell_type::directory,
                horizontal_alignment::left, vertical_alignment::center);
        }
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::update()
    {
        _cells.clear();

        for (auto& node : _model) {
            std::visit(
                [this, &node](auto& item)
                {
                    add_cells(node.first, item, 0);
                },
                node.second);
        }

        invalidate();
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::apply_color_theme(const color_theme &theme)
    {
        _selected_color = theme.primary_light;
        _hoverred_color = theme.secondary_light;
        _default_color = theme.on_surface;
    }

    template<typename Key, typename Value, typename Derived>
    bool directory_view<Key, Value, Derived>::on_mouse_wheel(float distance)
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

    template<typename Key, typename Value, typename Derived>
    bool directory_view<Key, Value, Derived>::on_mouse_move(float x, float y)
    {
        if (!cell_at(y, _hoverred_cell))
            _hoverred_cell = _cells.size();
        invalidate();
        return true;
    }

    template<typename Key, typename Value, typename Derived>
    bool directory_view<Key, Value, Derived>::on_mouse_button_up(const mouse_button button, float x, float y)
    {
        unsigned int idx;
        if (button == mouse_button::left && cell_at(y, idx)) {
            on_cell_click(idx);
            return true;
        }
        else {
            return false;
        }
    }

    template<typename Key, typename Value, typename Derived>
    bool directory_view<Key, Value, Derived>::on_mouse_drag_end(const mouse_button button, float x, float y)
    {
        unsigned int idx;
        if (button == mouse_button::left && cell_at(y, idx)) {
            on_cell_click(idx);
            return true;
        }
        else {
            return false;
        }
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::add_cells(const Key &k, model &dir, unsigned int level)
    {
        _cells.emplace_back(&dir, level, caption_by_key(k), cell_type::directory);

        //  if this directory is open
        if (_open_dirs.count(&dir) != 0) {

            for (auto& node : dir) {
                std::visit(
                    [this, &node, level](auto& item)
                    {
                        add_cells(node.first, item, level + 1);
                    },
                    node.second);
            }
        }
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::add_cells(const Key &k, const Value &, unsigned int level)
    {
        _cells.emplace_back(nullptr, level, caption_by_key(k), cell_type::value);
    }

    template<typename Key, typename Value, typename Derived>
    void directory_view<Key, Value, Derived>::on_cell_click(const unsigned int idx)
    {
        const auto& c = _cells[idx];

        if (c.type == cell_type::directory && c.dir != nullptr) {
            //  Swap open state
            if (is_open(c))
                _open_dirs.erase(c.dir);
            else
                _open_dirs.insert(c.dir);

            update();
        }
        else
        {
            /** \todo Click on value event **/
        }
    }

    template<typename Key, typename Value, typename Derived>
    bool directory_view<Key, Value, Derived>::cell_at(float y, unsigned int& idx)
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
