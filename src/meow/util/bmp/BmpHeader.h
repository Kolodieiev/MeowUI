#pragma once
#include <Arduino.h>

namespace meow
{

#pragma pack(push, 1)

    struct BmpHeader
    {
        uint16_t file_type{0};   // Тип файлу, має бути 'BM' (0x4D42)
        uint32_t file_size{0};   // Розмір файлу в байтах
        uint16_t reserved1{0};   // Зарезервоване поле, має бути 0
        uint16_t reserved2{0};   // Зарезервоване поле, має бути 0
        uint32_t data_offset{0}; // Зсув до початку даних
        //
        uint32_t size{0};   // Розмір даних структури в байтах
        uint32_t width{0};  // Ширина зображення в пікселях
        uint32_t height{0}; // Висота зображення в пікселях
        uint16_t planes{1};
        uint16_t bit_count{0};
        uint32_t compression{0};
        uint32_t image_size{0}; // Розмір зображення в байтах
        uint32_t x_pixels{0};
        uint32_t y_pixels{0};
        uint32_t colors_used{0};
        uint32_t colors_important{0};
    };

#pragma pack(pop)

}