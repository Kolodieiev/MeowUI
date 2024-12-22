#pragma GCC optimize("O3")

#include "IScreen.h"
#include "../widget/layout/EmptyLayout.h"

namespace meow
{
    Input IScreen::_input;

    void IScreen::tick()
    {
        static unsigned long upd_time = 0;

        loop();

        if ((millis() - upd_time) > UI_UPDATE_DELAY)
        {
            upd_time = millis();

            _input.update();

            update();

            if (_screen_enabled)
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

#pragma region "don't touch this"

    IScreen::IScreen(GraphicsDriver &display) : _display{display}
    {
        _layout_mutex = xSemaphoreCreateMutex();
        _layout = new EmptyLayout(1, _display);
        _layout->setBackColor(TFT_YELLOW);
        _layout->setWidth(_display.width());
        _layout->setHeight(_display.height());
    }

    IScreen::~IScreen()
    {
        delete _layout;
    }

    void IScreen::setLayout(IWidgetContainer *layout)
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

    void IScreen::openScreenByID(ScreenID screen_ID)
    {
        _input.reset();
        _next_screen_ID = screen_ID;
        _is_released = true;
    }

#pragma endregion "don't touch this"
}