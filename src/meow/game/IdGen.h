#pragma once
#include <Arduino.h>

namespace meow
{
    class IdGen
    {
    public:
        // Повертає унікальний ID в межах лічильника
        static uint32_t genID() { return ++_curr_id; }
        // Скидає лічильник ID
        static void reset() { _curr_id = 0; }

    private:
        static uint32_t _curr_id;
    };
}