#pragma GCC optimize("O3")

#include "ToggleItem.h"
namespace meow
{
    ToggleItem::ToggleItem(uint16_t widget_ID, GraphicsDriver &display) : MenuItem(widget_ID, display) {}

    ToggleItem::~ToggleItem()
    {
        delete _toggle;
    }

    void ToggleItem::onDraw()
    {
        if (!_is_changed)
        {
            if (_ico)
                _ico->onDraw();
            if (_label)
                _label->onDraw();
            if (_toggle)
                _toggle->onDraw();
            return;
        }

        _is_changed = false;

        clear();

        uint8_t img_width{0};
        uint8_t toggle_width{0};

        if (_toggle)
        {
            toggle_width = _toggle->getWidth() + ITEM_PADDING;
            _toggle->setPos(_width - ITEM_PADDING - _toggle->getWidth(), (_height - _toggle->getHeight()) * 0.5);

            _toggle->onDraw();
        }

        if (_ico)
        {
            img_width = _ico->getWidth() + ITEM_PADDING;
            _ico->setPos(ITEM_PADDING, (_height - _ico->getHeight()) * 0.5);

            if (_has_focus)
                _ico->setFocus();
            else
                _ico->removeFocus();

            _ico->onDraw();
        }

        if (_label)
        {
            _label->setHeight(_height - 2);
            _label->setPos(img_width + ITEM_PADDING, 1);
            _label->setWidth(_width - ITEM_PADDING * 2 - img_width - toggle_width);

            if (_has_focus)
                _label->setFocus();
            else
                _label->removeFocus();

            _label->onDraw();
        }
    }

    ToggleItem *ToggleItem::clone(uint16_t id) const
    {
        ToggleItem *clone = new ToggleItem(*this);

        if (!clone)
        {
            log_e("Помилка клонування");
            esp_restart();
        }

        clone->_id = id;

        if (_ico)
            clone->setIco(_ico->clone(_ico->getID()));

        if (_label)
            clone->setLbl(_label->clone(_label->getID()));

        if (_toggle)
            clone->setToggle(_toggle->clone(_toggle->getID()));

        return clone;
    }

    void ToggleItem::setToggle(ToggleSwitch *toggle)
    {
        if (!toggle)
        {
            log_e("ToggleSwitch не може бути null");
            esp_restart();
        }

        delete _toggle;

        _toggle = toggle;
        _is_changed = true;

        _toggle->setParent(this);
        _toggle->setChangingBorder(false);
    }

    void ToggleItem::on()
    {
        if (_toggle)
            _toggle->on();
    }

    void ToggleItem::off()
    {
        if (_toggle)
            _toggle->off();
    }

    bool ToggleItem::isOn() const
    {
        if (!_toggle)
            return false;

        return _toggle->isOn();
    }
}