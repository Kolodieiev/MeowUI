#include "Keyboard.h"

namespace meow
{
    Keyboard::Keyboard(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display) {}

    Keyboard *Keyboard::clone(uint16_t id) const
    {
        Keyboard *clone = new Keyboard(id, _display);

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

        for (uint16_t i{0}; i < _widgets.size(); ++i)
        {
            IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
            clone->addWidget(item);
        }

        return clone;
    }

    uint16_t Keyboard::getCurrentBtnID() const
    {
        return getFocusRow()->getCurrentBtnID();
    }

    String Keyboard::getCurrentBtnTxt() const
    {
        return getFocusRow()->getCurrentBtnTxt();
    }

    void Keyboard::focusUp()
    {
        KeyboardRow *row = getFocusRow();

        uint16_t focusPos = row->getCurFocusPos();

        row->removeFocus();

        if (_cur_focus_row_pos > 0)
            _cur_focus_row_pos--;
        else
            _cur_focus_row_pos = _widgets.size() - 1;

        row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
        row->setFocus(focusPos);
    }

    void Keyboard::focusDown()
    {
        KeyboardRow *row = getFocusRow();

        uint16_t focusPos = row->getCurFocusPos();

        row->removeFocus();

        if (_cur_focus_row_pos < _widgets.size() - 1)
            _cur_focus_row_pos++;
        else
            _cur_focus_row_pos = 0;

        row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
        row->setFocus(focusPos);
    }

    void Keyboard::focusLeft()
    {
        KeyboardRow *row = getFocusRow();

        if (row->focusUp())
            return;

        row->removeFocus();
        row->setFocus(row->getSize() - 1);
    }

    void Keyboard::focusRight()
    {
        KeyboardRow *row = getFocusRow();

        if (row->focusDown())
            return;

        row->removeFocus();
        row->setFocus(0);
    }

    KeyboardRow *Keyboard::getFocusRow() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної KeyboardRow");
            esp_restart();
        }

        KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

        if (!row)
        {
            log_e("KeyboardRow не знайдено");
            esp_restart();
        }

        return row;
    }

    void Keyboard::onDraw()
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

            if (_first_drawing)
            {
                _first_drawing = false;

                if (!_widgets.empty())
                    getFocusRow()->setFocus(0);
            }

            uint16_t x{2};
            uint16_t y{2};

            for (uint16_t i{0}; i < _widgets.size(); ++i)
            {
                _widgets[i]->setPos(x, y);
                _widgets[i]->setWidth(_width - 4);
                _widgets[i]->onDraw();
                y += _widgets[i]->getHeight();
            }
        }
    }
}