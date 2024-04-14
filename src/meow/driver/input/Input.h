#pragma once
#include <Arduino.h>
#include "Pin.h"
#include <map>

namespace meow
{

#define TOUCH true
#define TACT false

    class Input
    {
    public:
        //----------------------------------------------------------------------------------------------------------------  Відредагуй це
        enum PinID : uint8_t
        {
            PIN_UP = 38,
            PIN_LEFT = 39,
            PIN_RIGHT = 40,
            PIN_DOWN = 41,

            PIN_D = 9,
            PIN_A = 5,
            PIN_SELECT = 0,
            PIN_C = 10,
            PIN_B = 6,
            PIN_START = 4
        };

#pragma region "don't touch this"
        void update();
        // Скинути значення всіх пінів
        void reset();

        inline bool isHolded(PinID pin_id) { return _buttons[pin_id]->isHolded(); }
        inline bool isPressed(PinID pin_id) { return _buttons[pin_id]->isPressed(); }
        inline bool isReleased(PinID pin_id) { return _buttons[pin_id]->isReleased(); }
        inline void lock(PinID pin_id, unsigned long lock_duration) { _buttons[pin_id]->lock(lock_duration); }

#pragma endregion "don't touch this"

    private:
        //---------------------------------------------------------------------------------------------------------------- Відредагуй це
        std::map<PinID, Pin *> _buttons = {
            {PIN_UP, new Pin(PIN_UP, TACT)},
            {PIN_DOWN, new Pin(PIN_DOWN, TACT)},
            {PIN_LEFT, new Pin(PIN_LEFT, TACT)},
            {PIN_RIGHT, new Pin(PIN_RIGHT, TACT)},

            {PIN_A, new Pin(PIN_A, TACT)},
            {PIN_D, new Pin(PIN_D, TACT)},
            {PIN_B, new Pin(PIN_B, TACT)},
            {PIN_C, new Pin(PIN_C, TACT)},
            {PIN_START, new Pin(PIN_START, TACT)},
            {PIN_SELECT, new Pin(PIN_SELECT, TACT)}};
    };

}