#include "Menu.h"

namespace meow
{

    Menu::Menu(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display) {}

    IWidget *Menu::findItemByID(uint16_t itemID) const
    {
        IWidget *widget = findWidgetByID(itemID);

        if (widget == nullptr)
            return nullptr;
        else
            return widget;
    }

    void Menu::deleteWidgets()
    {
        _first_item_index = 0;
        _cur_focus_pos = 0;

        IWidgetContainer::deleteWidgets();
    }

    uint16_t Menu::getCurrentItemID() const
    {
        if (_widgets.size() == 0)
            return 0;

        return _widgets[_cur_focus_pos]->getID();
    }

    String Menu::getCurrentItemText() const
    {
        if (_widgets.size() == 0)
            return "";

        Label *item = reinterpret_cast<Label *>(_widgets[_cur_focus_pos]);
        return item->getText();
    }

    IWidget *Menu::getCurrentItem()
    {
        if (_widgets.size() == 0)
            return nullptr;

        return _widgets[_cur_focus_pos];
    }

    void Menu::onDraw()
    {
        if (!_is_changed)
        {
            if (_is_enabled && _visibility != INVISIBLE)
                for (uint16_t i{_first_item_index}; i < _first_item_index + getCyclesCount(); ++i)
                    _widgets[i]->onDraw();
        }
        else
        {
            _is_changed = false;

            if (_visibility == INVISIBLE)
            {
                hide();
                return;
            }

            if (_widgets.size() == 0)
                return;

            uint16_t cyclesCount = getCyclesCount();

            if (_first_item_index >= _widgets.size())
                _first_item_index = _widgets.size() - 1;

            if (_cur_focus_pos >= _widgets.size())
                _cur_focus_pos = _widgets.size() - 1;

            IWidget *item = _widgets[_cur_focus_pos];
            item->setFocus();

            drawItems(_first_item_index, cyclesCount);
        }
    }

    void Menu::drawItems(uint16_t start, uint16_t count)
    {
        clear();

        if (_widgets.size() == 0)
            return;

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent != nullptr)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        uint16_t itemXPos = 2;
        uint16_t itemYPos = 2;

        for (uint16_t i{start}; i < start + count; ++i)
        {
            _widgets[i]->setPos(itemXPos, itemYPos);

            if (_orientation == ORIENTATION_HORIZONTAL)
            {
                _widgets[i]->setHeight(_item_height);
                _widgets[i]->setWidth(_width - 4);
                itemYPos += (_item_height + _items_spacing);
            }
            else
            {
                _widgets[i]->setHeight(_height - 4);
                _widgets[i]->setWidth(_item_width);
                itemXPos += (_item_width + _items_spacing);
            }

            _widgets[i]->onDraw();
        }
    }

    uint16_t Menu::getCyclesCount() const
    {
        uint16_t cyclesCount;

        if (_orientation == ORIENTATION_HORIZONTAL)
            cyclesCount = (float)_height / (_item_height + _items_spacing);
        else
            cyclesCount = (float)_width / (_item_width + _items_spacing);

        uint16_t itemsToEndNum = _widgets.size() - _first_item_index;

        if (cyclesCount > itemsToEndNum)
            cyclesCount = itemsToEndNum;

        return cyclesCount;
    }

}