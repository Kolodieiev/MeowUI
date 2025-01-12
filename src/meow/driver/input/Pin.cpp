#pragma GCC optimize("O3")

#include "Pin.h"
#include "driver/touch_sensor.h"
#include "esp32-hal-touch.h"
#include "hal/gpio_hal.h"
#include "meowui_setup/input_setup.h"

namespace meow
{
    Pin::Pin(uint8_t pin_id, bool is_touch) : _pin_id{pin_id},
                                              _is_touch{is_touch}
    {
        enable();
    }

    void Pin::lock(unsigned long lock_duration)
    {
        _is_locked = true;
        _is_holded = false;
        _is_released = false;
        _is_pressed = false;
        _is_touched = false;
        _lock_duration = lock_duration;
        _lock_time = millis();
    }

    void Pin::_update()
    {
        if (!_is_enabled)
            return;

        if (_is_locked)
        {
            if (millis() - _lock_time < _lock_duration)
                return;
            _is_locked = false;
        }

        if (_is_touch)
        {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
            static uint32_t touch_value;
#else
            static uint16_t touch_value;
#endif
            touch_pad_read_raw_data((touch_pad_t)_pad, &touch_value);

#if defined(CONFIG_IDF_TARGET_ESP32S3)
            _is_holded = touch_value > KEY_TOUCH_TRESHOLD;
#else
            _is_holded = touch_value < KEY_TOUCH_TRESHOLD;
#endif
        }
        else
        {
            // TODO тест на esp32
#if defined(CONFIG_IDF_TARGET_ESP32S3)
            _is_holded = (_pin_id < 32) ? !((GPIO.in >> _pin_id) & 1) : !((GPIO.in1.val >> (_pin_id - 32)) & 1);
#else
            _is_holded = !gpio_get_level((gpio_num_t)_pin_id);
#endif
        }

        if (!_is_holded)
        {
            if (_is_touched)
            {
                _is_touched = false;
                _is_released = true;
            }

            return;
        }

        if (_is_touched)
        {
            if (!_is_pressed)
                if (millis() - _hold_duration > PRESS_DURATION)
                {
                    _is_touched = false;
                    _is_pressed = true;
                }
        }
        else
        {
            _is_touched = true;
            _is_released = false;
            _hold_duration = millis();
        }
    }

    void Pin::reset()
    {
        _is_holded = false;
        _is_pressed = false;
        _is_released = false;
    }

    void Pin::enable()
    {
        _is_enabled = true;

        if (!_is_touch)
        {
            pinMode(_pin_id, INPUT_PULLUP);
        }
        else
        {
            touchRead(_pin_id); // init channel
            _pad = digitalPinToTouchChannel(_pin_id);
        }
    }

    void Pin::disable()
    {
        _is_enabled = false;
        reset();

        gpio_reset_pin((gpio_num_t)_pin_id);
        pinMode(_pin_id, INPUT);
    }
}