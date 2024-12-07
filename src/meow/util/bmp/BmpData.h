#pragma once
#include <Arduino.h>
#include "./BmpHeader.h"

namespace meow
{
    struct BmpData
    {
        bool is_loaded{false};
        uint16_t width{0};
        uint16_t height{0};
        const uint16_t *data_ptr{nullptr};
    };
}