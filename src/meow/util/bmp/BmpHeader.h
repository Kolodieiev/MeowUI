#pragma once
#include <Arduino.h>

namespace meow
{

#pragma pack(push, 1)

    struct BmpHeader
    {
        uint16_t file_type{0x4D42};
        uint32_t file_size{0};
        uint16_t reserved1{0};
        uint16_t reserved2{0};
        uint32_t data_offset{0};

        uint32_t size{0};
        int32_t width{0};
        int32_t height{0};
        uint16_t planes{1};
        uint16_t bit_count{0};
        uint32_t compression{0};
        uint32_t image_size{0};
        int32_t x_pixels_per_meter{0};
        int32_t y_pixels_per_meter{0};
        uint32_t colors_used{0};
        uint32_t colors_important{0};
    };

#pragma pack(pop)

}