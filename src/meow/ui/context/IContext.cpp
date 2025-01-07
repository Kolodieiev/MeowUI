#pragma GCC optimize("O3")

#include "IContext.h"
#include "../widget/layout/EmptyLayout.h"

namespace meow
{
    Input IContext::_input;

    void IContext::_tick()
    {
        static unsigned long upd_time = 0;

        loop();

        if ((millis() - upd_time) > UI_UPDATE_DELAY)
        {
            upd_time = millis();

            _input.update();

            update();

            if (_gui_enabled)
            {
                xSemaphoreTake(_layout_mutex, portMAX_DELAY);
                _layout->onDraw();
                xSemaphoreGive(_layout_mutex);

#ifdef DOUBLE_BUFFERRING
                _display.pushBuffer();
#endif
            }
        }
    }

    IContext::IContext(GraphicsDriver &display) : _display{display}
    {
        _layout_mutex = xSemaphoreCreateMutex();
        _layout = new EmptyLayout(1, _display);
        _layout->setBackColor(TFT_YELLOW);
        _layout->setWidth(_display.width());
        _layout->setHeight(_display.height());
    }

    IContext::~IContext()
    {
        delete _layout;
    }

    void IContext::setLayout(IWidgetContainer *layout)
    {
        if (!layout)
        {
            log_e("Спроба встановити NULL-layout.");
            esp_restart();
        }

        if (_layout == layout)
            return;

        xSemaphoreTake(_layout_mutex, portMAX_DELAY);

        delete _layout;
        _layout = layout;

        xSemaphoreGive(_layout_mutex);
    }

    void IContext::openContextByID(ContextID screen_ID)
    {
        _input.reset();
        _next_context_ID = screen_ID;
        _is_released = true;
    }
}