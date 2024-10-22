#include "DynamicMenu.h"

namespace meow
{

    DynamicMenu::DynamicMenu(IItemsLoader *loader, uint16_t widget_ID, GraphicsDriver &display) : Menu(widget_ID, display), _loader{loader} {}

    bool DynamicMenu::focusUp()
    {
        if (!_widgets.empty())
        {
            if (_cur_focus_pos)
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

                return true;
            }
            else
            {
                std::vector<IWidget *> temp_vector = _loader->loadPrev(getItemsNumberOnScreen(), _widgets[_cur_focus_pos]->getID());

                if (temp_vector.size() > 0)
                {
                    deleteWidgets();

                    _widgets = temp_vector;

                    drawItems(_first_item_index, getCyclesCount());
                    _cur_focus_pos = _widgets.size() - 1;

                    IWidget *item = _widgets[_cur_focus_pos];
                    item->setFocus();

                    return true;
                }
            }
        }

        return false;
    }

    bool DynamicMenu::focusDown()
    {
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

                return true;
            }
            else
            {
                std::vector<IWidget *> temp_vector = _loader->loadNext(getItemsNumberOnScreen(), _widgets[_cur_focus_pos]->getID());

                if (temp_vector.size() > 0)
                {
                    deleteWidgets();

                    _widgets = temp_vector;

                    drawItems(_first_item_index, getCyclesCount());
                    _cur_focus_pos = _first_item_index;

                    IWidget *item = _widgets[_cur_focus_pos];
                    item->setFocus();

                    return true;
                }
            }
        }

        return false;
    }

    uint16_t DynamicMenu::getItemsNumberOnScreen() const
    {
        return (float)_height / _item_height;
    }

    DynamicMenu *DynamicMenu::clone(uint16_t id) const
    {
        DynamicMenu *clone = new DynamicMenu(_loader, id, IWidgetContainer::_display);

        if (!clone)
        {
            log_e("Помилка клонування");
            esp_restart();
        }

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

        for (uint16_t i{0}; i < _widgets.size(); ++i)
        {
            IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
            clone->addWidget(item);
        }

        return clone;
    }

}