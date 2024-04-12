#include "KeyboardRow.h"
#include "../text/Label.h"

namespace meow
{

    KeyboardRow::KeyboardRow(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display) {}

    KeyboardRow *KeyboardRow::clone(uint16_t id) const
    {
        KeyboardRow *clone = new KeyboardRow(id, _display);

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
        clone->_btn_height = _btn_height;
        clone->_btn_width = _btn_width;

        for (uint16_t i{0}; i < _widgets.size(); ++i)
        {
            IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
            clone->addWidget(item);
        }

        return clone;
    }

    uint16_t KeyboardRow::getCurrentBtnID() const
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return 0;
        }

        if (_cur_focus_pos > _widgets.size() - 1)
        {
            log_e("Кнопку не знайдено.");
            return 0;
        }

        return _widgets[_cur_focus_pos]->getID();
    }

    String KeyboardRow::getCurrentBtnTxt() const
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return "";
        }

        if (_cur_focus_pos > _widgets.size() - 1)
        {
            log_e("Кнопку не знайдено.");
            return "";
        }

        Label *item = reinterpret_cast<Label *>(_widgets[_cur_focus_pos]);

        if (item == nullptr)
        {
            log_e("Кнопку не знайдено.");
            return "";
        }

        return item->getText();
    }

    bool KeyboardRow::focusUp()
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return false;
        }

        if (_cur_focus_pos > 0)
        {
            IWidget *btn = _widgets[_cur_focus_pos];

            if (btn == nullptr)
            {
                log_e("Кнопку не знайдено.");
                return false;
            }

            btn->removeFocus();

            _cur_focus_pos--;

            btn = reinterpret_cast<Label *>(_widgets[_cur_focus_pos]);
            btn->setFocus();

            return true;
        }

        return false;
    }

    bool KeyboardRow::focusDown()
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return false;
        }

        if (_cur_focus_pos < _widgets.size() - 1)
        {
            IWidget *btn = _widgets[_cur_focus_pos];

            if (btn == nullptr)
            {
                log_e("Кнопку не знайдено.");
                return false;
            }

            btn->removeFocus();

            _cur_focus_pos++;

            btn = _widgets[_cur_focus_pos];
            btn->setFocus();

            return true;
        }

        return false;
    }

    void KeyboardRow::setFocus(uint16_t pos)
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return;
        }

        IWidget *btn = _widgets[_cur_focus_pos];

        if (btn == nullptr)
        {
            log_e("Кнопку не знайдено.");
            return;
        }

        btn->removeFocus();

        if (pos > _widgets.size() - 1)
            _cur_focus_pos = _widgets.size() - 1;
        else
            _cur_focus_pos = pos;

        btn = _widgets[_cur_focus_pos];
        btn->setFocus();
    }

    void KeyboardRow::removeFocus()
    {
        if (_widgets.size() == 0)
        {
            log_e("Не додано жодної кнопки.");
            return;
        }

        IWidget *item = _widgets[_cur_focus_pos];

        if (item == nullptr)
        {
            log_e("Кнопку не знайдено.");
            return;
        }

        item->removeFocus();

        _cur_focus_pos = 0;
    }

    void KeyboardRow::onDraw()
    {
        if (!_is_changed)
        {
            if (_visibility != INVISIBLE)
                for (uint16_t i{0}; i < _widgets.size(); ++i)
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

            uint16_t step = (_width - _btn_width * _widgets.size()) / (_widgets.size() + 1);

            uint16_t x = _x_pos + step;
            uint16_t y = _y_pos;

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