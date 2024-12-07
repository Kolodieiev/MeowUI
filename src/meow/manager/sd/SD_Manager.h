#pragma once
#include <Arduino.h>
#include <SD.h>

namespace meow
{
    class SD_Manager
    {
    public:
        void setup(uint8_t cs_pin = SS, SPIClass *spi = &SPI, uint32_t frequency = 4000000U, const char *mountpoint = "/sd", uint8_t max_files = (uint8_t)5U);
        bool testConection() const;
        bool mount();
        void unmount();
        static SD_Manager getInstance();

    private:
        static bool _is_inited;

        bool _is_mounted = false;
        bool _has_setup = false;

        uint8_t _cs_pin;
        SPIClass *_spi;
        uint32_t _frequency;
        const char *_mountpoint;
        uint8_t _max_files;

        SD_Manager(){}
    };

    extern SD_Manager SD_MNGR;
}