#pragma GCC optimize("O3")

#include "DynamicMenu.h"

namespace meow
{
    DynamicMenu::DynamicMenu(IItemsLoader *loader, uint16_t widget_ID, GraphicsDriver &display) : Menu(widget_ID, display), _loader{loader} {}

    bool DynamicMenu::focusUp()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty())
        {
            if (_cur_focus_pos > 0)
            {
                IWidget *item = _widgets[_cur_focus_pos];
                item->removeFocus();

                --_cur_focus_pos;

                uint16_t cycles_count = getCyclesCount();

                if (_cur_focus_pos < _first_item_index)
                {
                    --_first_item_index;
                    drawItems(_first_item_index, cycles_count);
                }

                item = _widgets[_cur_focus_pos];
                item->setFocus();

                xSemaphoreGive(_widg_mutex);
                return true;
            }
            else
            {
                std::vector<MenuItem *> temp_vec = _loader->loadPrev(getItemsNumOnScreen(), _widgets[_cur_focus_pos]->getID());
                if (!temp_vec.empty())
                {
                    deleteWidgets();
                    _widgets.reserve(temp_vec.size());
                    for (uint16_t i{0}; i < temp_vec.size(); ++i)
                        _widgets.push_back(temp_vec[i]);

                    drawItems(_first_item_index, getCyclesCount());
                    _cur_focus_pos = _widgets.size() - 1;

                    _widgets[_cur_focus_pos]->setFocus();

                    xSemaphoreGive(_widg_mutex);
                    return true;
                }
            }
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    bool DynamicMenu::focusDown()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty())
        {
            if (_cur_focus_pos < _widgets.size() - 1)
            {
                IWidget *item = _widgets[_cur_focus_pos];
                item->removeFocus();

                ++_cur_focus_pos;

                uint16_t cycles_count = getCyclesCount();

                if (_cur_focus_pos > _first_item_index + cycles_count - 1)
                {
                    ++_first_item_index;

                    drawItems(_first_item_index, cycles_count);
                }

                item = _widgets[_cur_focus_pos];
                item->setFocus();

                xSemaphoreGive(_widg_mutex);
                return true;
            }
            else
            {
                std::vector<MenuItem *> temp_vec = _loader->loadNext(getItemsNumOnScreen(), _widgets[_cur_focus_pos]->getID());
                if (!temp_vec.empty())
                {
                    deleteWidgets();
                    _widgets.reserve(temp_vec.size());
                    for (uint16_t i{0}; i < temp_vec.size(); ++i)
                        _widgets.push_back(temp_vec[i]);

                    drawItems(_first_item_index, getCyclesCount());
                    _cur_focus_pos = _first_item_index;

                    _widgets[_cur_focus_pos]->setFocus();

                    xSemaphoreGive(_widg_mutex);
                    return true;
                }
            }
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    uint16_t DynamicMenu::getItemsNumOnScreen() const
    {
        return (float)_height / _item_height;
    }

    DynamicMenu *DynamicMenu::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            DynamicMenu *clone = new DynamicMenu(_loader, id, IWidgetContainer::_display);

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