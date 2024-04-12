#include "EmptyLayout.h"

namespace meow
{

    EmptyLayout::EmptyLayout(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display) {}

    EmptyLayout::~EmptyLayout() {}

    void EmptyLayout::onDraw()
    {
        if (!_is_changed)
        {
            if (_is_enabled && _visibility != INVISIBLE)
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

            for (uint16_t i{0}; i < _widgets.size(); ++i)
                _widgets[i]->forcedDraw();
        }
    }

    EmptyLayout *EmptyLayout::clone(uint16_t id) const
    {
        EmptyLayout *clone = new EmptyLayout(id, IWidgetContainer::_display);

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

}