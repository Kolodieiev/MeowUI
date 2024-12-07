#pragma GCC optimize("O3")

#include "IWidget.h"

namespace meow
{
    IWidget::IWidget(uint16_t widget_ID, GraphicsDriver &display) : _id{widget_ID}, _display{display} {}

    IWidget::~IWidget() {}

    void IWidget::forcedDraw()
    {
        _is_changed = true;
        onDraw();
    }

    void IWidget::clear() const
    {
        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent != nullptr)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (!_corner_radius)
            _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _back_color);
        else
            _display.fillRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _back_color);

        if (_has_border)
        {
            if (!_corner_radius)
                _display.drawRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _border_color);
            else
                _display.drawRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _border_color);
        }
    }

    void IWidget::hide() const
    {
        uint16_t back_color{0};
        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
            back_color = _parent->getBackColor();
        }

        if (!_corner_radius)
            _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, back_color);
        else
            _display.fillRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, back_color);
    }

    uint16_t IWidget::getXPos() const
    {
        if (_parent)
            return _parent->getXPos() + _x_pos;
        else
            return _x_pos;
    }

    uint16_t IWidget::getYPos() const
    {
        if (_parent)
            return _parent->getYPos() + _y_pos;
        else
            return _y_pos;
    }

    void IWidget::setFocus()
    {
        if (_has_focus)
            return;

        _is_changed = true;
        _has_focus = true;

        if (_need_change_border)
        {
            _old_border_state = _has_border;
            _has_border = true;

            _old_border_color = _border_color;
            _border_color = _focus_border_color;
        }

        if (_need_change_back)
        {
            _old_back_color = _back_color;
            _back_color = _focus_back_color;
        }
    }

    void IWidget::removeFocus()
    {
        if (!_has_focus)
            return;

        _is_changed = true;
        _has_focus = false;

        if (_need_change_border)
        {
            _has_border = _old_border_state;
            _border_color = _old_border_color;
        }

        if (_need_change_back)
            _back_color = _old_back_color;
    }

    bool IWidget::hasIntersectWithCoords(uint16_t x, uint16_t y)
    {
        if (_parent)
            return (x > _parent->getXPos() + _x_pos && x < _parent->getXPos() + _x_pos + _width) &&
                   (y > _parent->getYPos() + _y_pos && y < _parent->getYPos() + _y_pos + _height);
        else
            return (x > _x_pos && x < _x_pos + _width) && (y > _y_pos && y < _y_pos + _height);
    }
}