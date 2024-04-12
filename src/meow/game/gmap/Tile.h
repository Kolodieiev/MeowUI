#pragma once
#include <Arduino.h>

namespace meow
{

    class Tile
    {
    public:
        enum TileType : uint8_t
        {
            TYPE_NONE = 0,
            TYPE_GROUND,
            TYPE_WATER,
            TYPE_MUD,
            TYPE_AIR,
            TYPE_WALL
        };

        Tile(TileType type, const uint16_t *img_ptr) : _type{type}, _img_ptr{img_ptr} {}

        TileType _type;
        const uint16_t *_img_ptr;
    };

}