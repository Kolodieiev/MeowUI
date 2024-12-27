#pragma once
#include <stdint.h>

#define SD_SPI_BUS HSPI

#define SD_PIN_CS (int8_t)16
#define SD_PIN_MISO (int8_t)8
#define SD_PIN_MOSI (int8_t)17
#define SD_PIN_SCLK (int8_t)18

#define SD_FREQUENCY 80000000
#define SD_MOUNTPOINT "/sd"
#define SD_MAX_FILES (uint8_t)255
