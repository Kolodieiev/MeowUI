#pragma once
#include <Arduino.h>

namespace meow
{

    struct IObjShape
    {
        virtual ~IObjShape() = 0;

        int16_t class_ID;

        uint16_t x_global;
        uint16_t y_global;

        bool is_triggered;
        bool id_destroed;
    };

}