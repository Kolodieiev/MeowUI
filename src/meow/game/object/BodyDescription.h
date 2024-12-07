#pragma once
#include <Arduino.h>

namespace meow
{

    struct BodyDescription
    {
        bool is_rigid{true}; // Чи має об'єкт тверде тіло
        uint16_t ofst_h{0};  // Зміщення rigid body від верхнього краю спрайта
        uint16_t ofst_w{0};  // Зміщення rigid body від лівого/правого краю

        uint8_t pass_abillity_mask{0}; // Маска, яка вказує, по якому типу поверхні може переміщуватися об'єкт.
    };

}