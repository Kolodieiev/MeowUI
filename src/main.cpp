#include <Arduino.h>
#include "SD.h"
#include "meow/Meow.h"

#define SCK_PIN 18
#define MISO_PIN 8
#define MOSI_PIN 17
#define SD_CS_PIN 16

// Апаратний SPI
// #define SCK 12
// #define MISO 13
// #define MOSI 11

void guiTask(void *params)
{
    meow::MEOW.run();
}

void setup()
{
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

    if (!SD.begin(SD_CS_PIN, SPI, 80000000))
        log_e("Помилка ініціалізації SD");

    xTaskCreatePinnedToCore(guiTask, "guiTask", (1024 / 2) * 100, NULL, 10, NULL, 1);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}