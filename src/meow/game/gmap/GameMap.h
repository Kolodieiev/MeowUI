#pragma once
#include <Arduino.h>
#include <cmath>
#include <unordered_map>
//
#include "../../driver/graphics/GraphicsDriver.h"
//
#include "Tile.h"

#include "../object/BodyDescription.h"
#include "../object/SpriteDescription.h"

namespace meow
{
    class GameMap
    {
    public:
        const uint16_t VIEW_W; // Ширина view-порта
        const uint16_t VIEW_H; // Висота view-порта
        //
        uint16_t _map_w; // Ширина мапи в пікселях
        uint16_t _map_h; // Висота мапи в пікселях
        //
        uint16_t _view_x{0}; // Поточні координати view-порта
        uint16_t _view_y{0}; // Поточні координати view-порта
        //
        GameMap(GraphicsDriver &display) : _display{display},
                                           VIEW_W{display.width()},
                                           VIEW_H{display.height()} {}
        ~GameMap();

        // Відрисувати мапу з указаних координат, якщо можливо
        // Інакше відрисувати частину мапу найближчу до цих координат
        void onDraw();

        // Зібрати мапу
        void build(uint16_t tiles_x_num, uint16_t tiles_y_num, uint16_t tile_side_len, const uint16_t *map_build_table);

        // Чи можна пройти в указанному напрямку
        bool canPass(uint16_t x_from, uint16_t y_from, int32_t x_to, int32_t y_to, const BodyDescription &body, const SpriteDescription &sprite);

        // Отримати тип плитки в указаних координатах
        Tile::TileType getTileType(uint16_t x, uint16_t y);

        // Встановити позицію камери
        void setCameraPos(uint16_t x, uint16_t y);
        // Перевірити, чи потрапляє об'єкт до view-порту
        bool isInView(uint16_t x_pos, uint16_t y_pos, uint16_t sprite_w, uint16_t sprite_h);
        //
        void addTileDescr(uint16_t sprite_id, Tile::TileType type, const uint16_t *sprite_src);

    private:
        std::unordered_map<uint16_t, Tile *> _tile_descr; // Опис плиток.

        Tile ***_tiles{nullptr}; // Ігрова мапа.

        uint8_t _tile_side_len; // Розмір сторони плитки
        uint16_t _tile_x_num;   // Кількість плиток мапи по горизонталі
        uint16_t _tile_y_num;   // Кількість плиток мапи по вертикалі

        GraphicsDriver &_display;

        // Звільнити ресурси
        void freeMem();

        // Перетворити глобальну позицію до позиції плитки
        uint16_t getTilePos(uint16_t obj_pos);
    };

}