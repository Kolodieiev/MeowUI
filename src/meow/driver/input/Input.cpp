#pragma GCC optimize("O3")

#include "Input.h"

#ifdef GT911_DRIVER
#include "./touchscreen_driver/GT911.h"
#endif

namespace meow
{
    Input::Input()
    {
#ifdef TOUCHSCREEN_SUPPORT
#ifdef GT911_DRIVER
        GT911 *gt = new GT911();
        gt->setRotation(TOUCH_ROTATION);
        gt->begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_INT_PIN, TOUCH_RST_PIN, TOUCH_WIDTH, TOUCH_HEIGHT);
        _touchscreen = gt;
#endif // GT911_DRIVER

#endif // TOUCHSCREEN_SUPPORT
    }

    void Input::update()
    {
#ifdef TOUCHSCREEN_SUPPORT
        _touchscreen->update();
#endif

        for (auto &&btn : _buttons)
            btn.second->update();
    }

    void Input::reset()
    {
#ifdef TOUCHSCREEN_SUPPORT
        _touchscreen->reset();
#endif

        for (auto &&btn : _buttons)
            btn.second->reset();
    }
}