#pragma once
#include <Arduino.h>
#pragma GCC optimize("O3")

#include "meowui_setup/input_setup.h"

#include "Pin.h"
#include "ITouchscreen.h"
#include <unordered_map>

namespace meow
{
    class Input
    {
    public:
        Input();
        // Оновити стан вводу. Не потрібно викликати метод самостійно
        void update();
        // Скинути стан вводу
        void reset();

        void enablePin(KeyID key_id) { _buttons.at(key_id)->enable(); }
        void disablePin(KeyID key_id) { _buttons.at(key_id)->disable(); }
        bool isHolded(KeyID key_id) const { return _buttons.at(key_id)->isHolded(); }
        bool isPressed(KeyID key_id) const { return _buttons.at(key_id)->isPressed(); }
        bool isReleased(KeyID key_id) const { return _buttons.at(key_id)->isReleased(); }
        void lock(KeyID key_id, unsigned long lock_duration) { _buttons.at(key_id)->lock(lock_duration); }

#ifdef TOUCHSCREEN_SUPPORT
        bool isHolded() const { return _touchscreen->isHolded(); }
        bool isPressed() const { return _touchscreen->isPressed(); }
        bool isReleased() const { return _touchscreen->isReleased(); }
        bool isSwiped() const { return _touchscreen->isSwiped(); }
        void lock(unsigned long lock_duration) { _touchscreen->lock(lock_duration); }

        ITouchscreen::Swipe getSwipe() { return _touchscreen->getSwipe(); }
        uint16_t getX() const { return _touchscreen->getX(); }
        uint16_t getY() const { return _touchscreen->getY(); }
#endif

    private:
        std::unordered_map<KeyID, Pin *> _buttons = BUTTONS;

#ifdef TOUCHSCREEN_SUPPORT
        ITouchscreen *_touchscreen;
#endif
    };
}