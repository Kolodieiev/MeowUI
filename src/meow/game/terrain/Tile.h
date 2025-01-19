#pragma once
#include <Arduino.h>

namespace meow
{
    class Tile
    {
    public:
        // Типи поверхні плитки. Можете розширити власними типами, за потреби.
        enum TileType : uint8_t
        {
            TYPE_NONE = 0,
            TYPE_WALL,
            TYPE_GROUND,
            TYPE_WATER,
            TYPE_AIR,
            TYPE_FIRE,
            TYPE_SNOW,
            TYPE_MUD,
        };

        Tile(TileType type, const uint16_t *img_ptr) : _type{type}, _img_ptr{img_ptr} {}

        const TileType _type;
        const uint16_t *_img_ptr;
    };
}