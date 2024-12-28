#pragma GCC optimize("O3")

#include "FixedMenu.h"

namespace meow
{
    FixedMenu::FixedMenu(uint16_t widget_ID, GraphicsDriver &display) : Menu(widget_ID, display) {}

    bool FixedMenu::focusUp()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty())
        {
            IWidget *item = _widgets[_cur_focus_pos];
            item->removeFocus();
            uint16_t cycles_count = getCyclesCount();

            bool need_redraw = false;

            if (_cur_focus_pos > 0)
            {
                --_cur_focus_pos;

                if (_cur_focus_pos < _first_item_index)
                {
                    --_first_item_index;
                    need_redraw = true;
                }
            }
            else if (_is_loop_enbl)
            {
                if (_widgets.size() > cycles_count)
                {
                    need_redraw = true;
                    _first_item_index = _widgets.size() - cycles_count;
                }
                else
                    _first_item_index = 0;

                _cur_focus_pos = _widgets.size() - 1;
            }

            item = _widgets[_cur_focus_pos];
            item->setFocus();

            if (need_redraw)
                drawItems(_first_item_index, cycles_count);

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    bool FixedMenu::focusDown()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty())
        {
            IWidget *item = _widgets[_cur_focus_pos];
            item->removeFocus();
            uint16_t cycles_count = getCyclesCount();

            bool need_redraw = false;

            if (_cur_focus_pos < _widgets.size() - 1)
            {
                ++_cur_focus_pos;

                if (_cur_focus_pos > _first_item_index + cycles_count - 1)
                {
                    need_redraw = true;
                    ++_first_item_index;
                }
            }
            else if (_is_loop_enbl)
            {
                _cur_focus_pos = 0;
                _first_item_index = 0;

                need_redraw = _widgets.size() > cycles_count;
            }

            item = _widgets[_cur_focus_pos];
            item->setFocus();

            if (need_redraw)
                drawItems(_first_item_index, cycles_count);

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    FixedMenu *FixedMenu::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            FixedMenu *clone = new FixedMenu(id, IWidgetContainer::_display);

            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_item_height = _item_height;
            clone->_item_width = _item_width;
            clone->_items_spacing = _items_spacing;
            clone->_is_loop_enbl = _is_loop_enbl;
            clone->_is_transparent = _is_transparent;

            for (const auto &widget_ptr : _widgets)
            {
                IWidget *item = widget_ptr->clone(widget_ptr->getID());
                clone->addWidget(item);
            }

            xSemaphoreGive(_widg_mutex);
            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}