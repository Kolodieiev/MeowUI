#pragma once
#include <Arduino.h>
#include "meowui_setup/display_setup.h"

namespace meow
{
    class DisplayUtil
    {
    public:
#ifdef BACKLIGHT_PIN
        void enableBackLight();
        void disableBackLight();
        void setBrightness(uint8_t value);
        uint8_t getBrightness() const { return _cur_brightness; }

    private:
        uint8_t _cur_brightness = 125;
#endif
    };

}