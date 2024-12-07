#pragma GCC optimize("O3")

#include "KeyboardRow.h"
#include "../text/Label.h"

namespace meow
{
    KeyboardRow::KeyboardRow(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display) {}

    KeyboardRow *KeyboardRow::clone(uint16_t id) const
    {
        try
        {
            KeyboardRow *clone = new KeyboardRow(id, _display);

            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_btn_height = _btn_height;
            clone->_btn_width = _btn_width;

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

    uint16_t KeyboardRow::getCurrentBtnID() const
    {
        return getFocusBtn()->getID();
    }

    String KeyboardRow::getCurrentBtnTxt() const
    {
        return reinterpret_cast<Label *>(getFocusBtn())->getText();
    }

    bool KeyboardRow::focusUp()
    {
        if (_cur_focus_pos > 0)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            --_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();
            return true;
        }

        return false;
    }

    bool KeyboardRow::focusDown()
    {
        if (!_widgets.empty() && _cur_focus_pos < _widgets.size() - 1)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            ++_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();
            return true;
        }

        return false;
    }

    void KeyboardRow::setFocus(uint16_t pos)
    {
        IWidget *btn = getFocusBtn();

        btn->removeFocus();

        if (pos > _widgets.size() - 1)
            _cur_focus_pos = _widgets.size() - 1;
        else
            _cur_focus_pos = pos;

        btn = getFocusBtn();
        btn->setFocus();
    }

    void KeyboardRow::removeFocus()
    {
        getFocusBtn()->removeFocus();
        _cur_focus_pos = 0;
    }

    IWidget *KeyboardRow::getFocusBtn() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної кнопки");
            esp_restart();
        }

        IWidget *item = _widgets[_cur_focus_pos];

        if (!item)
        {
            log_e("Кнопку не знайдено");
            esp_restart();
        }

        return item;
    }

    void KeyboardRow::onDraw()
    {
        if (!_is_changed)
        {
            if (_visibility != INVISIBLE)
                for (uint16_t i{0}; _is_enabled && i < _widgets.size(); ++i)
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

            clear();

            if (_widgets.empty())
                return;

            uint16_t step{0};

            if (_btn_width * _widgets.size() < _width)
                step = (float)(_width - _btn_width * _widgets.size()) / (_widgets.size() + 1);

            uint16_t x = step;
            uint16_t y = (float)(_height - _btn_height) / 2;

            for (uint16_t i{0}; i < _widgets.size(); ++i)
            {
                _widgets[i]->setPos(x, y);
                _widgets[i]->setWidth(_btn_width);
                _widgets[i]->setHeight(_btn_height);
                _widgets[i]->onDraw();

                x += _btn_width + step;
            }
        }
    }
}