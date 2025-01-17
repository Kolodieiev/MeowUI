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

        if (!_is_transparent)
            clear();

        uint8_t toggle_width{0};
        if (_toggle)
        {
            toggle_width = _toggle->getWidth() + ITEM_PADDING;
            _toggle->setPos(_width - ITEM_PADDING - _toggle->getWidth(), (_height - _toggle->getHeight()) * 0.5);

            _toggle->onDraw();
        }

        uint8_t img_width{0};
        if (_ico)
        {
            _ico->setParent(this);
            img_width = _ico->getWidth() + ITEM_PADDING;
            _ico->setPos(ITEM_PADDING, (_height - _ico->getHeight()) * 0.5);

            uint16_t bk_color = _back_color;

            if (_has_focus && _need_change_back)
                _ico->setBackColor(_focus_back_color);

            _ico->setBackColor(bk_color);

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
        try
        {
            ToggleItem *clone = new ToggleItem(*this);

            clone->_id = id;

            if (_ico)
                clone->setIco(_ico->clone(_ico->getID()));

            if (_label)
                clone->setLbl(_label->clone(_label->getID()));

            if (_toggle)
                clone->setToggle(_toggle->clone(_toggle->getID()));

            return clone;
        }
        catch (const std::bad_alloc &e)
        {

            log_e("%s", e.what());
            esp_restart();
        }
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