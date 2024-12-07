#pragma once
#include <Arduino.h>
#include <vector>
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{

    struct SpriteDescription
    {
        uint16_t width{1};  // Ширина спрайту
        uint16_t height{1}; // Висота спрайту

        bool has_animation{false}; // Чи має об'єкт анімацію спрайта
        bool has_img{false};       // Чи має об'єкт зображення для спрайта

        uint8_t frames_between_anim{0}; // Кількість пропущених кадрів між кадрами анімації

        uint16_t transp_color{0xF81F};    // Колір прозорості спрайту
        const uint16_t *img_ptr{nullptr}; // Вказівник на массив із зображенням, якщо відсутня анімація

        uint8_t anim_pos_counter{0};             // Лічильник кадрів
        uint8_t anim_pos{0};                     // Номер поточного кадру анімації
        std::vector<const uint16_t *> *cur_anim; // Поточна анімація

        uint16_t x_pivot{1}; // Х-координата обертання спрайта
        uint16_t y_pivot{1}; // Y-координата обертання спрайта
        int16_t angle{0};    // Кут повороту спрайта

        // Зкидає лічильник кадрів, та лічильник поточної анімації
        inline void reset()
        {
            anim_pos_counter = 0;
            anim_pos = 0;
        }
    };

}