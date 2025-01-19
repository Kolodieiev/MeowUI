#pragma GCC optimize("O3")
#include "TerrainManager.h"

namespace meow
{
    TerrainManager::~TerrainManager()
    {
        freeMem();
    }

    void TerrainManager::freeMem()
    {
        if (!_terrain)
            return;

        for (uint16_t i = 0; i < _tile_y_num; ++i)
            free(_terrain[i]);

        free(_terrain);
        _terrain = nullptr;

        freeTilesDescriptionData();
    }

    void TerrainManager::freeTilesDescriptionData()
    {
        for (auto it = _tile_descr.begin(), last_it = _tile_descr.end(); it != last_it; ++it)
            delete it->second;

        _tile_descr.clear();
    }

    void TerrainManager::setCameraPos(uint16_t x, uint16_t y)
    {
        // X
        if (x < HALF_VIEW_W)
            _view_x = 0;
        else if (x < _terrain_w - HALF_VIEW_W)
            _view_x = x - HALF_VIEW_W;
        else
            _view_x = _terrain_w - VIEW_W;

        // Y
        if (y < HALF_VIEW_H)
            _view_y = 0;
        else if (y < _terrain_h - HALF_VIEW_H)
            _view_y = y - HALF_VIEW_H;
        else
            _view_y = _terrain_h - VIEW_H;
    }

    uint16_t TerrainManager::coordToTilePos(uint16_t obj_pos)
    {
        float temp_float_div = (float)obj_pos / _tile_side_len;
        uint16_t rounded_div = round(temp_float_div);

        if (rounded_div > temp_float_div)
            --rounded_div;

        return rounded_div;
    }

    void TerrainManager::onDraw()
    {
        if (_back_img)
            _display.pushImage(0, 0, VIEW_W, VIEW_H, _back_img);

        if (_terrain)
        {
            uint16_t first_tile_y_pos = coordToTilePos(_view_y);
            uint16_t first_tile_x_pos = coordToTilePos(_view_x);

            uint16_t last_tile_x_pos = coordToTilePos(_view_x + VIEW_W);
            uint16_t last_tile_y_pos = coordToTilePos(_view_y + VIEW_H);

            ++last_tile_y_pos;
            ++last_tile_x_pos;

            if (last_tile_y_pos > _tile_y_num)
                last_tile_y_pos = _tile_y_num;

            if (last_tile_x_pos > _tile_x_num)
                last_tile_x_pos = _tile_x_num;

            const int32_t x_draw_pos = (_view_x / _tile_side_len) * _tile_side_len - _view_x;
            int32_t y_draw_pos = (_view_y / _tile_side_len) * _tile_side_len - _view_y;

            int32_t temp_x_draw_pos = x_draw_pos;

            for (uint16_t h = first_tile_y_pos; h < last_tile_y_pos; ++h)
            {
                for (uint16_t w = first_tile_x_pos; w < last_tile_x_pos; ++w)
                {
                    if (_terrain[h][w]->_img_ptr)
                        _display.pushImage(temp_x_draw_pos, y_draw_pos, _tile_side_len, _tile_side_len, _terrain[h][w]->_img_ptr);
                    temp_x_draw_pos += _tile_side_len;
                }

                temp_x_draw_pos = x_draw_pos;
                y_draw_pos += _tile_side_len;
            }
        }
    }

    void TerrainManager::buildTerrain(uint16_t tiles_x_num, uint16_t tiles_y_num, uint16_t tile_side_len, const uint16_t *tiles_pos_table)
    {
        if (tiles_x_num == 0 || tiles_y_num == 0 || tile_side_len == 0 || !tiles_pos_table)
        {
            log_e("Некоректні параметри");
            esp_restart();
        }

        if (_tile_descr.empty())
        {
            log_e("Відсутній опис плиток ігрового рівня");
            esp_restart();
        }

        if (!psramInit())
        {
            log_e("Відсутня память PSRAM");
            esp_restart();
        }

        _terrain_w = tiles_x_num * tile_side_len;
        _terrain_h = tiles_y_num * tile_side_len;
        //
        _tile_side_len = tile_side_len;
        //
        _tile_x_num = tiles_x_num;
        _tile_y_num = tiles_y_num;

        freeMem(); // Звільнити ресурси, якщо було викликано не перший раз

        _terrain = (Tile ***)ps_malloc(sizeof(Tile **) * _tile_y_num);

        if (!_terrain)
        {
            log_e("Помилка виділення пам'яті");
            esp_restart();
        }

        for (uint16_t i{0}; i < _tile_y_num; ++i)
        {
            _terrain[i] = (Tile **)ps_malloc(sizeof(Tile *) * _tile_x_num);

            if (!_terrain[i])
            {
                log_e("Помилка виділення пам'яті");
                esp_restart();
            }
        }

        uint32_t build_pos{0};
        for (uint16_t i{0}; i < _tile_y_num; ++i)
        {
            for (uint16_t j{0}; j < _tile_x_num; ++j)
            {
                uint16_t sprite_id = tiles_pos_table[build_pos];
                _terrain[i][j] = _tile_descr.at(sprite_id);
                ++build_pos;
            }
        }

        freeTilesDescriptionData();
    }

    bool TerrainManager::canPass(uint16_t x_from, uint16_t y_from, int32_t x_to, int32_t y_to, const SpriteDescription &sprite)
    {
        if (x_to < 0 || y_to < 0 || x_to > _terrain_w || y_to > _terrain_h || !_terrain)
            return false;

        if (!sprite.is_rigid)
            return true;

        // Якщо рух вгору чи вниз, додатково перевірити по ширині.
        // Якщо вправо чи вліво, по висоті.

        if (y_to < y_from) // UP
        {
            uint16_t tile_y_pos = coordToTilePos(y_to + sprite.ofst_h);      // Висота куди планується переміщення
            uint16_t tile_x_main_pos = coordToTilePos(x_to + sprite.ofst_w); // х лівого краю

            if (_terrain[tile_y_pos][tile_x_main_pos]->_type & sprite.pass_abillity_mask) // чи проходить лівий кут спрайта
            {
                uint16_t tile_x_side_pos = coordToTilePos(x_to + sprite.width - 1 - sprite.ofst_w); // х правого краю
                return _terrain[tile_y_pos][tile_x_side_pos]->_type & sprite.pass_abillity_mask;    // Якщо другий теж проходить значить спрайт проходить повністю
            }
        }
        else if (y_to > y_from) // DOWN
        {
            uint16_t tile_y_pos = coordToTilePos(y_to + sprite.height - 1); // Висота куди повинна стати нижня сторона спрайту

            if (tile_y_pos >= _tile_y_num)
                return false;

            uint16_t tile_x_main_pos = coordToTilePos(x_to + sprite.ofst_w); // х лівого краю
            if (_terrain[tile_y_pos][tile_x_main_pos]->_type & sprite.pass_abillity_mask)
            {
                uint16_t tile_x_side_pos = coordToTilePos(x_to + sprite.width - 1 - sprite.ofst_w); // х правого краю

                return _terrain[tile_y_pos][tile_x_side_pos]->_type & sprite.pass_abillity_mask;
            }
        }
        else if (x_to < x_from) // LEFT
        {
            uint16_t tile_x_pos = coordToTilePos(x_to + sprite.ofst_w);      // х куди стане ліва сторона спрайту
            uint16_t tile_y_main_pos = coordToTilePos(y_to + sprite.ofst_h); // у верхнього краю

            if (_terrain[tile_y_main_pos][tile_x_pos]->_type & sprite.pass_abillity_mask)
            {
                uint16_t tile_y_side_pos = coordToTilePos(y_to + sprite.height - 1); // у нижнього краю
                return _terrain[tile_y_side_pos][tile_x_pos]->_type & sprite.pass_abillity_mask;
            }
        }
        else if (x_to > x_from) // RIGHT
        {
            uint16_t tile_x_pos = coordToTilePos(x_to + sprite.width - 1 - sprite.ofst_w); // Права сторона спрайту

            if (tile_x_pos >= _tile_x_num)
                return false;

            uint16_t tile_y_main_pos = coordToTilePos(y_to + sprite.ofst_h); // у верхнього краю
            if (_terrain[tile_y_main_pos][tile_x_pos]->_type & sprite.pass_abillity_mask)
            {
                uint16_t tile_y_side_pos = coordToTilePos(y_to + sprite.height - 1); // у нижнього краю
                return _terrain[tile_y_side_pos][tile_x_pos]->_type & sprite.pass_abillity_mask;
            }
        }

        return false;
    }

    Tile::TileType TerrainManager::getTileType(uint16_t x, uint16_t y)
    {
        if (x > _terrain_w || y > _terrain_h || !_terrain)
            return Tile::TYPE_NONE;

        uint16_t tile_y_pos = coordToTilePos(y);
        uint16_t tile_x_pos = coordToTilePos(x);
        return _terrain[tile_y_pos][tile_x_pos]->_type;
    }

    bool TerrainManager::isInView(uint16_t x_pos, uint16_t y_pos, uint16_t sprite_w, uint16_t sprite_h)
    {
        if (x_pos + sprite_w < _view_x || y_pos + sprite_h < _view_y || x_pos > _view_x + VIEW_W || y_pos > _view_y + VIEW_H)
            return false;

        return true;
    }

    void TerrainManager::addTileDescr(uint16_t sprite_id, Tile::TileType type, const uint16_t *sprite_src)
    {
        try
        {
            Tile *tile = new Tile(type, sprite_src);
            _tile_descr.insert(std::pair<uint16_t, Tile *>(sprite_id, tile));
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}