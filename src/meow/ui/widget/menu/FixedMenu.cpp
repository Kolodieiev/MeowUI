#pragma GCC optimize("O3")

#include "FixedMenu.h"

namespace meow
{

    FixedMenu::FixedMenu(uint16_t widget_ID, GraphicsDriver &display) : Menu(widget_ID, display) {}

    bool FixedMenu::focusUp()
    {
        if (!_widgets.empty())
            if (_cur_focus_pos > 0)
            {
                IWidget *item = _widgets[_cur_focus_pos];
                item->removeFocus();

                _cur_focus_pos--;

                uint16_t cycles_count = getCyclesCount();

                if (_cur_focus_pos < _first_item_index)
                {
                    _first_item_index--;
                    drawItems(_first_item_index, cycles_count);
                }

                item = _widgets[_cur_focus_pos];
                item->setFocus();

                return true;
            }

        return false;
    }

    bool FixedMenu::focusDown()
    {
        if (!_widgets.empty())
            if (_cur_focus_pos < _widgets.size() - 1)
            {
                IWidget *item = _widgets[_cur_focus_pos];
                item->removeFocus();

                _cur_focus_pos++;

                uint16_t cycles_count = getCyclesCount();

                if (_cur_focus_pos > _first_item_index + cycles_count - 1)
                {
                    _first_item_index++;

                    drawItems(_first_item_index, cycles_count);
                }

                item = _widgets[_cur_focus_pos];
                item->setFocus();

                return true;
            }

        return false;
    }

    FixedMenu *FixedMenu::clone(uint16_t id) const
    {
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

            for (const auto &widget_ptr : _widgets)
            {
                IWidget *item = widget_ptr->clone(widget_ptr->getID());
                clone->addWidget(item);
            }

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            
            log_e("%s", e.what());
            esp_restart();
        }
    }

}