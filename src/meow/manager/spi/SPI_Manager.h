#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <unordered_map>

namespace meow
{
    class SPI_Manager
    {
    public:
        bool initBus(uint8_t bus_num, uint8_t sclk_pin, uint8_t miso_pin, uint8_t mosi_pin);
        void deinitBus(uint8_t bus_num);
        SPIClass *getSpi4Bus(uint8_t bus_num);

    private:
        static std::unordered_map<uint8_t, SPIClass *> _spi_map;
    };
}