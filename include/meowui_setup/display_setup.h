#pragma once
#include <stdint.h>

#include "User_Setups/Setup_lilka_v2_st7789.h" // Файл налаштування драйвера дисплею. 
// #include <User_Setups/Setup303_ST7735_custom.h>


// #define BACKLIGHT_PIN (uint8_t)39  // Пін управління підсвіткою дисплея. Закоментуй, якщо відсутнє управління підсвіткою
#define PWM_FREQ (uint16_t)3000 // Частота PWM підсвітки дисплея
#define PWM_CHANEL (uint8_t)0
#define PWM_RESOLUTION (uint8_t)8
