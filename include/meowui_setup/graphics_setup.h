#pragma once

#define DISPLAY_ROTATION 3 // Стартова орієнтація екрану

#define DOUBLE_BUFFERRING // Подвійна буферизація. Працює тільки за наявності PSRAM. Інакше буде викликано рестарт МК

// #define SHOW_FPS // Відображати значення FPS на дисплеї

// -------------------------------- Глибина кольору зображення
#define COLOR_16BIT
// #define COLOR_8BIT
// #define COLOR_1BIT

// #define ENABLE_SCREENSHOTER // Увімкнути підтримку створення скриншотів. Тримай закоментованим, якщо не використовується
