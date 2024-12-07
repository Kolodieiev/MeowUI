#pragma GCC optimize("O3")

#include "SpinBox.h"

namespace meow
{

    SpinBox::SpinBox(uint16_t widget_ID, GraphicsDriver &display) : Label(widget_ID, display)
    {
        setAlign(IWidget::ALIGN_CENTER);
        setGravity(IWidget::GRAVITY_CENTER);
    }

    SpinBox *SpinBox::clone(uint16_t id) const
    {
        try
        {
            SpinBox *clone = new SpinBox(*this);
            clone->_id = id;
            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void SpinBox::setMin(float min)
    {
        _min = min;

        if (_value < _min)
        {
            _value = _min;
        }

        setSpinValToDraw();
        _is_changed = true;
    }

    void SpinBox::setMax(float max)
    {
        _max = max;
        if (_value > _max)
        {
            _value = _max;
        }

        setSpinValToDraw();
        _is_changed = true;
    }

    void SpinBox::setValue(float value)
    {
        if (value < _min)
            _value = _min;
        else if (value > _max)
            _value = _max;
        else
            _value = value;

        setSpinValToDraw();
    }

    void SpinBox::setType(SpinType spin_type)
    {
        _spin_type = spin_type;
        _is_changed = true;
        setSpinValToDraw();
    }

    void SpinBox::setSpinValToDraw()
    {
        if (_spin_type == TYPE_INT)
        {
            uint64_t temp = _value;
            setText(String(temp));
        }
        else
            setText(String(_value));
    }

    void SpinBox::up()
    {
        if (_value < _max)
            _value++;
        else
            _value = _min;

        setSpinValToDraw();
    }

    void SpinBox::down()
    {
        if (_value > _min)
            _value--;
        else
            _value = _max;

        setSpinValToDraw();
    }

}