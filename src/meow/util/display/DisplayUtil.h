#pragma once
#include <Arduino.h>

namespace meow
{

#define BLIGHT_PIN 4  // Пін управління підсвіткою дисплея
#define PWM_FREQ 3000 // Частота PWM підсвітки дисплея

    class Util
    {
    public:
        static void setBrightness(uint8_t value)
        {
            const uint8_t PWM_CHANEL{0};
            const uint8_t PWM_RESOLUTION{8};

            ledcSetup(PWM_CHANEL, PWM_FREQ, PWM_RESOLUTION);
            ledcAttachPin(BLIGHT_PIN, PWM_CHANEL);
            ledcWrite(PWM_CHANEL, value);
        }
    };

}