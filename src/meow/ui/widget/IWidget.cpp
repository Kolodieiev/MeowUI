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
        if (_parent == nullptr)
            return;

        uint16_t x_offset = _parent->getXPos();
        uint16_t y_offset = _parent->getYPos();

        if (!_corner_radius)
            _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _parent->getBackColor());
        else
            _display.fillRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _parent->getBackColor());
    }

    void IWidget::setFocus()
    {
        if (_has_focus)
            return;

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

        _is_changed = true;
    }

    void IWidget::removeFocus()
    {
        if (!_has_focus)
            return;

        _has_focus = false;

        if (_need_change_border)
        {
            _has_border = _old_border_state;
            _border_color = _old_border_color;
        }

        if (_need_change_back)
            _back_color = _old_back_color;

        _is_changed = true;
    }

}