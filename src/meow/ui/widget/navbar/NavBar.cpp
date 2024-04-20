#include "NavBar.h"

namespace meow
{

    NavBar::NavBar(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display) {}

    NavBar::~NavBar()
    {
        if (_first != nullptr)
        {
            delete _first;
            delete _middle;
            delete _last;
        }
    }

    void NavBar::setWidgets(IWidget *first, IWidget *middle, IWidget *last)
    {
        if (first == nullptr || middle == nullptr || last == nullptr)
        {
            log_e("Спроба передати NULL-об'єкт.");
            esp_restart();
        }

        if (_first)
        {
            delete _first;
            delete _middle;
            delete _last;
        }

        _first = first;
        _middle = middle;
        _last = last;

        _first->setParent(this);
        _middle->setParent(this);
        _last->setParent(this);

        _is_changed = true;
    }

    void NavBar::onDraw()
    {
        if (_is_changed)
        {
            _is_changed = false;

            if (_visibility == INVISIBLE)
            {
                hide();
                return;
            }

            clear();

            if (_first == nullptr)
                return;

            uint16_t x_offset{0};
            uint16_t y_offset{0};

            if (_parent != nullptr)
            {
                x_offset = _parent->getXPos();
                y_offset = _parent->getYPos();
            }

            _first->setPos(_x_pos, _y_pos);
            _first->setBackColor(_back_color);
            _first->setHeight(_height);
            _first->onDraw();

            _middle->setPos(_x_pos + x_offset + (_width - _middle->getWidth()) / 2, _y_pos + y_offset);
            _middle->setBackColor(_back_color);
            _middle->setHeight(_height);
            _middle->onDraw();

            _last->setPos(_x_pos + x_offset + _width - _last->getWidth(), _y_pos + y_offset);
            _last->setBackColor(_back_color);
            _last->setHeight(_height);
            _last->onDraw();
        }
    }

    NavBar *NavBar::clone(uint16_t id) const
    {
        NavBar *clone = new NavBar(*this);

        if (!clone)
        {
            log_e("Помилка клонування");
            esp_restart();
        }

        clone->_id = id;

        if (_first)
        {
            clone->setWidgets(
                _first->clone(_first->getID()),
                _middle->clone(_middle->getID()),
                _last->clone(_last->getID()));
        }

        return clone;
    }

}