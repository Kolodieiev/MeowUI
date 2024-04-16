#pragma once
#include <Arduino.h>

namespace meow
{
    struct BmpHeader
    {
        uint16_t file_type{0x4D42};    // Тип файлу, має бути 'BM' (0x4D42)
        uint32_t file_size{0};         // Розмір файлу в байтах
        uint16_t reserved1{0};         // Зарезервоване поле, має бути 0
        uint16_t reserved2{0};         // Зарезервоване поле, має бути 0
        uint32_t offset_data{0};       // Зсув до початку масиву піксельних даних
        //
        uint32_t size{0};              // Розмір даних структури нижче в байтах
        int32_t width{0};              // Ширина зображення в пікселях
        int32_t height{0};             // Висота зображення в пікселях
        uint16_t planes{1};            // Кількість плоскостей зображення, зазвичай 1
        uint16_t bit_count{0};         // Кількість біт на піксель
        uint32_t compression{0};       // Тип компресії
        uint32_t size_image{0};        // Розмір зображення в байтах
        int32_t x_pixels_per_meter{0}; // Роздільна здатність по горизонталі
        int32_t y_pixels_per_meter{0}; // Роздільна здатність по вертикалі
        uint32_t colors_used{0};       // Кількість кольорів у палітрі
        uint32_t colors_important{0};  // Кількість важливих кольорів
    };
}