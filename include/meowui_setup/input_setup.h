#pragma once
#include <stdint.h>

// -------------------------------- Увімкнути підтримку сенсорного екрану
// #define TOUCHSCREEN_SUPPORT

#ifdef TOUCHSCREEN_SUPPORT
#define TOUCH_ROTATION ROTATION_0
#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

// -------------------------------- Обрати драйвер сенсорного екрану
#define GT911_DRIVER // Поки що додано підтримку тільки 1 чіпа

// -------------------------------- Налаштуй піни, до яких підключено драйвер тачскріна
#ifdef GT911_DRIVER
#define TOUCH_SDA_PIN 33
#define TOUCH_SCL_PIN 32
#define TOUCH_INT_PIN 21
#define TOUCH_RST_PIN 25
#endif // GT911_DRIVER

#endif // TOUCHSCREEN_SUPPORT

// -------------------------------- Закріплення назв кнопок до виводів МК

namespace meow
{
    enum KeyID : uint8_t
    {
        KEY_UP = 38,
        KEY_LEFT = 39,
        KEY_RIGHT = 40,
        KEY_DOWN = 41,

        KEY_D = 9,
        KEY_A = 5,
        KEY_C = 10,
        KEY_B = 6,

        KEY_START = 4,
        KEY_SELECT = 0,
    };
}

// -------------------------------- Макрос з ініціалізацією пінів. Між фігурними дужками потрібно додати ініціалізацію пінів, як показано нижче
// -------------------------------- Де Pin(uint8_t key_id, bool is_touch)
#define BUTTONS                                   \
    {                                             \
        {KEY_UP, new Pin(KEY_UP, false)},         \
        {KEY_DOWN, new Pin(KEY_DOWN, false)},     \
        {KEY_LEFT, new Pin(KEY_LEFT, false)},     \
        {KEY_RIGHT, new Pin(KEY_RIGHT, false)},   \
        {KEY_D, new Pin(KEY_D, false)},           \
        {KEY_A, new Pin(KEY_A, false)},           \
        {KEY_C, new Pin(KEY_C, false)},           \
        {KEY_B, new Pin(KEY_B, false)},           \
        {KEY_START, new Pin(KEY_START, false)},   \
        {KEY_SELECT, new Pin(KEY_SELECT, false)}, \
    }

// -------------------------------- Налаштування чутливості сенсорних кнопок
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define KEY_TOUCH_TRESHOLD 50000
#else
#define KEY_TOUCH_TRESHOLD 65
#endif

// -------------------------------- Час, після спливання якого дія вважається утримуваною.
#define PRESS_DURATION (unsigned long)1500
