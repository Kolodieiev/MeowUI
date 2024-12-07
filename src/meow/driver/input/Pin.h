#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class Pin
    {
    public:
        Pin(uint8_t pin, bool is_touch);

        void lock(unsigned long lock_duration);
        void update();
        void reset();

        inline uint8_t getID() const { return _pin_id; }
        inline bool isHolded() const { return _is_holded; }
        inline bool isPressed() const { return _is_pressed; }
        inline bool isReleased() const { return _is_released; }

        inline bool operator<(const Pin &other)
        {
            return _pin_id < other.getID();
        }

        void enable();
        void disable();

    private:
        const uint8_t _pin_id;
        const bool _is_touch;
        int8_t _pad;
        //
        bool _is_locked{false};
        unsigned long _lock_time;
        unsigned long _lock_duration;
        //
        bool _is_touched{false};
        unsigned long _hold_duration;
        //
        bool _is_holded{false};
        bool _is_pressed{false};
        bool _is_released{false};
        bool _is_enabled;
    };
}