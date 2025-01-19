#pragma once
#include <Arduino.h>

namespace meow
{
    class IdGen
    {
    public:
        /**
         * @brief Повертає унікальний ID в межах лічильника.
         *
         * @return uint32_t
         */
        static uint32_t genID() { return ++_curr_id; }

        /**
         * @brief Скидає лічильник ID.
         *
         */
        static void reset() { _curr_id = 0; }

    private:
        static uint32_t _curr_id;
    };
}