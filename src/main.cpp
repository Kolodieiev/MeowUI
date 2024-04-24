#include <Arduino.h>
#include "meow/ui/ScreenManager.h"

#define SCK_PIN 18
#define MISO_PIN 8
#define MOSI_PIN 17
#define SD_CS_PIN 16

#define SLEEP_PIN 46

// Апаратний SPI
// #define SCK 12
// #define MISO 13
// #define MOSI 11

void guiTask(void *params)
{
    // Serial.begin(115200);

    meow::MEOW.run();
}

void setup()
{
    
#if (ESP_ARDUINO_VERSION_MAJOR < 3)
#pragma message("Для коректної роботи бібліотеки, необхідно застосувати цей патч ")
    // Посилання на сторінку з патчем https://github.com/espressif/arduino-esp32/commit/629ffc55ed97b561f5bd1412a40cc83d00b2f825
    // Файл до якого потрібно застосувати патч C:\.platformio\packages\framework-arduinoespressif32\cores\esp32\esp32-hal-spi.c
#endif

    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, HIGH);
    //
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    //
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

    xTaskCreatePinnedToCore(guiTask, "guiTask", (1024 / 2) * 100, NULL, 10, NULL, 1);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}