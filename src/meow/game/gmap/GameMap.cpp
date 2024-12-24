#pragma GCC optimize("O3")
#include "GameMap.h"

namespace meow
{
    GameMap::~GameMap()
    {
        freeMem();
    }

    void GameMap::freeMem()
    {
        if (!_tiles)
            return;

        for (uint16_t i = 0; i < _tile_y_num; ++i)
            free(_tiles[i]);

        free(_tiles);

        for (auto it = _tile_descr.begin(), last_it = _tile_descr.end(); it != last_it; ++it)
            delete it->second;
    }

    void GameMap::setCameraPos(uint16_t x, uint16_t y)
    {
        uint16_t half_view_w = (float)VIEW_W * 0.5;
        uint16_t half_view_h = (float)VIEW_H * 0.5;

        if (x < half_view_w)
        {
            _view_x = 0;
        }
        else if (x < _map_w - half_view_w)
        {
            _view_x = x - half_view_w;
        }
        else
        {
            _view_x = _map_w - VIEW_W;
        }

        if (y < half_view_h)
        {
            _view_y = 0;
        }
        else if (y < _map_h - half_view_h)
        {
            _view_y = y - half_view_h;
        }
        else
        {
            _view_y = _map_h - VIEW_H;
        }
    }

    uint16_t GameMap::getTilePos(uint16_t obj_pos)
    {
        float temp_float_div = (float)obj_pos / _tile_side_len;
        uint16_t rounded_div = round(temp_float_div);

        if (rounded_div > temp_float_div)
            --rounded_div;

        return rounded_div;
    }

    void GameMap::onDraw()
    {
        uint16_t first_tile_y_pos = getTilePos(_view_y);
        uint16_t first_tile_x_pos = getTilePos(_view_x);

        uint16_t last_tile_x_pos = getTilePos(_view_x + VIEW_W);
        uint16_t last_tile_y_pos = getTilePos(_view_y + VIEW_H);

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
                _display.pushImage(temp_x_draw_pos, y_draw_pos, _tile_side_len, _tile_side_len, _tiles[h][w]->_img_ptr);
                temp_x_draw_pos += _tile_side_len;
            }

            temp_x_draw_pos = x_draw_pos;
            y_draw_pos += _tile_side_len;
        }
    }

    void GameMap::build(uint16_t tiles_x_num, uint16_t tiles_y_num, uint16_t tile_side_len, const uint16_t *map_build_table)
    {
        _map_w = tiles_x_num * tile_side_len;
        _map_h = tiles_y_num * tile_side_len;
        //
        _tile_side_len = tile_side_len;
        //
        _tile_x_num = tiles_x_num;
        _tile_y_num = tiles_y_num;

        freeMem(); // Звільнити ресурси, якщо було викликано не перший раз

        _tiles = (Tile ***)ps_malloc(sizeof(Tile **) * _tile_y_num);

        if (!_tiles)
        {
            log_e("Помилка виділення пам'яті");
            esp_restart();
        }

        for (uint16_t i{0}; i < _tile_y_num; ++i)
        {
            _tiles[i] = (Tile **)ps_malloc(sizeof(Tile *) * _tile_x_num);

            if (!_tiles[i])
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
                uint16_t sprite_id = map_build_table[build_pos];
                _tiles[i][j] = _tile_descr.at(sprite_id);
                ++build_pos;
            }
        }
    }

    bool GameMap::canPass(uint16_t x_from, uint16_t y_from, int32_t x_to, int32_t y_to, const BodyDescription &body, const SpriteDescription &sprite)
    {
        if (x_to < 0 || y_to < 0 || x_to > _map_w || y_to > _map_h)
            return false;

        if (!body.is_rigid)
            return true;

        // Якщо рух вгору чи вниз, додатково перевірити по ширині.
        // Якщо вправо чи вліво, по висоті.

        if (y_to < y_from) // UP
        {
            uint16_t tile_y_pos = getTilePos(y_to + body.ofst_h);      // Висота куди планується переміщення
            uint16_t tile_x_main_pos = getTilePos(x_to + body.ofst_w); // х лівого краю

            if (_tiles[tile_y_pos][tile_x_main_pos]->_type & body.pass_abillity_mask) // чи проходить лівий кут спрайта
            {
                uint16_t tile_x_side_pos = getTilePos(x_to + sprite.width - 1 - body.ofst_w); // х правого краю
                return _tiles[tile_y_pos][tile_x_side_pos]->_type & body.pass_abillity_mask;  // Якщо другий теж проходить значить спрайт проходить повністю
            }
        }
        else if (y_to > y_from) // DOWN
        {
            uint16_t tile_y_pos = getTilePos(y_to + sprite.height - 1); // Висота куди повинна стати нижня сторона спрайту

            if (tile_y_pos >= _tile_y_num)
                return false;

            uint16_t tile_x_main_pos = getTilePos(x_to + body.ofst_w); // х лівого краю
            if (_tiles[tile_y_pos][tile_x_main_pos]->_type & body.pass_abillity_mask)
            {
                uint16_t tile_x_side_pos = getTilePos(x_to + sprite.width - 1 - body.ofst_w); // х правого краю

                return _tiles[tile_y_pos][tile_x_side_pos]->_type & body.pass_abillity_mask;
            }
        }
        else if (x_to < x_from) // LEFT
        {
            uint16_t tile_x_pos = getTilePos(x_to + body.ofst_w);      // х куди стане ліва сторона спрайту
            uint16_t tile_y_main_pos = getTilePos(y_to + body.ofst_h); // у верхнього краю

            if (_tiles[tile_y_main_pos][tile_x_pos]->_type & body.pass_abillity_mask)
            {
                uint16_t tile_y_side_pos = getTilePos(y_to + sprite.height - 1); // у нижнього краю
                return _tiles[tile_y_side_pos][tile_x_pos]->_type & body.pass_abillity_mask;
            }
        }
        else if (x_to > x_from) // RIGHT
        {
            uint16_t tile_x_pos = getTilePos(x_to + sprite.width - 1 - body.ofst_w); // Права сторона спрайту

            if (tile_x_pos >= _tile_x_num)
                return false;

            uint16_t tile_y_main_pos = getTilePos(y_to + body.ofst_h); // у верхнього краю
            if (_tiles[tile_y_main_pos][tile_x_pos]->_type & body.pass_abillity_mask)
            {
                uint16_t tile_y_side_pos = getTilePos(y_to + sprite.height - 1); // у нижнього краю
                return _tiles[tile_y_side_pos][tile_x_pos]->_type & body.pass_abillity_mask;
            }
        }

        return false;
    }

    Tile::TileType GameMap::getTileType(uint16_t x, uint16_t y)
    {
        if (x > _map_w || y > _map_h)
            return Tile::TileType::TYPE_NONE;

        uint16_t tile_y_pos = getTilePos(y);
        uint16_t tile_x_pos = getTilePos(x);
        return _tiles[tile_y_pos][tile_x_pos]->_type;
    }

    bool GameMap::isInView(uint16_t x_pos, uint16_t y_pos, uint16_t sprite_w, uint16_t sprite_h)
    {
        if (x_pos + sprite_w < _view_x || y_pos + sprite_h < _view_y || x_pos > _view_x + VIEW_W || y_pos > _view_y + VIEW_H)
            return false;

        return true;
    }

    void GameMap::addTileDescr(uint16_t sprite_id, Tile::TileType type, const uint16_t *sprite_src)
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