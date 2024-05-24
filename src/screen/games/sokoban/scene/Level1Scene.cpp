#include "Level1Scene.h"

#include "../obj/TriggerID.h"
#include "../SceneID.h"

// Підключити шаблон мапи рівня
#include "../map/template/map_scene_1.h"

// Зображення плиток
#include "../map/tile_img/tile_0.h"
#include "../map/tile_img/tile_1.h"
//
#include "../ui/SceneUI.h"
// Ігрові об'єкти
#include "../obj/box/BoxObj.h"
#include "../obj/box_point/BoxPointObj.h"

Level1Scene::Level1Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded)
    : IGameScene(display, input, stored_objs)
{
    buildMap();

    createGhost();

    createSokoban();

    createBoxes();

    createBoxPoints();

    _game_UI = new SceneUI(_display);
}

Level1Scene::~Level1Scene()
{
    delete _ghost;
}

void Level1Scene::update()
{
    IGameObject::MovingDirection direction = IGameObject::DIRECTION_NONE;

    if (_input.isReleased(Input::PIN_START))
    {
        _input.lock(Input::PIN_START, 400);
        _is_finished = true;
        return;
    }

    if (_input.isReleased(Input::PIN_A))
    {
        _input.lock(Input::PIN_A, 400);
        openSceneByID(SceneID::ID_SCENE_LVL1);
        return;
    }

    if (_input.isReleased(Input::PIN_D))
    {
        _input.lock(Input::PIN_D, 400);
        _is_ghost_selected = !_is_ghost_selected;
        _input.reset(); 
    }

    if (_is_ghost_selected)
    {
        _main_obj = _ghost;

        if (_input.isHolded(Input::PIN_UP))
            direction = IGameObject::DIRECTION_UP;
        else if (_input.isHolded(Input::PIN_DOWN))
            direction = IGameObject::DIRECTION_DOWN;
        else if (_input.isHolded(Input::PIN_RIGHT))
            direction = IGameObject::DIRECTION_RIGHT;
        else if (_input.isHolded(Input::PIN_LEFT))
            direction = IGameObject::DIRECTION_LEFT;

        if (direction != IGameObject::DIRECTION_NONE)
            _ghost->move(direction);
    }
    else
    {
        _main_obj = _sokoban;

        if (_input.isReleased(Input::PIN_UP))
        {
            _input.lock(Input::PIN_UP, 100);
            direction = IGameObject::DIRECTION_UP;
        }
        else if (_input.isReleased(Input::PIN_DOWN))
        {
            _input.lock(Input::PIN_DOWN, 100);
            direction = IGameObject::DIRECTION_DOWN;
        }
        else if (_input.isReleased(Input::PIN_RIGHT))
        {
            _input.lock(Input::PIN_RIGHT, 100);
            direction = IGameObject::DIRECTION_RIGHT;
        }
        else if (_input.isReleased(Input::PIN_LEFT))
        {
            _input.lock(Input::PIN_LEFT, 100);
            direction = IGameObject::DIRECTION_LEFT;
        }

        if (direction != IGameObject::DIRECTION_NONE)
            _sokoban->move(direction);
    }

    IGameScene::update();
}

void Level1Scene::onTriggered(int16_t id)
{
    if (id == TriggerID::TRIGGER_NEXT_SCENE)
    {
        openSceneByID(SceneID::ID_SCENE_LVL2);
    }
}

void Level1Scene::buildMap()
{
    _game_map.addTileDescr(0, Tile::TYPE_GROUND, IMG_TILE_0);
    _game_map.addTileDescr(1, Tile::TYPE_WALL, IMG_TILE_1);

    uint8_t tiles_x_num{19};
    uint8_t tiles_y_num{14};
    uint8_t tile_width{32};

    _game_map.build(tiles_x_num, tiles_y_num, tile_width, map_scene_1);
}

void Level1Scene::createGhost()
{
    _ghost = createObject<GhostObj>();
    _ghost->init();
    _main_obj = _ghost;
}

void Level1Scene::createSokoban()
{
    _sokoban = createObject<SokobanObj>();
    _sokoban->init();
    _game_objs.push_back(_sokoban);
    _sokoban->_x_global = 8 * 32;
    _sokoban->_y_global = 10 * 32;
}

void Level1Scene::createBoxes()
{
    uint16_t box_arr[BOX_NUM][2] = {
        {2 * 32, 4 * 32},
        {3 * 32, 4 * 32},
        {4 * 32, 4 * 32},
        {3 * 32, 5 * 32},
        {2 * 32, 6 * 32},
        {3 * 32, 6 * 32},
        {3 * 32, 7 * 32},
        {3 * 32, 11 * 32},
        {4 * 32, 11 * 32},
        {6 * 32, 4 * 32},
        {6 * 32, 6 * 32},
        {8 * 32, 3 * 32},
        {8 * 32, 6 * 32},
        {9 * 32, 4 * 32},
        {9 * 32, 5 * 32},
        {10 * 32, 3 * 32},
        {10 * 32, 6 * 32},
        {6 * 32, 10 * 32},
        {6 * 32, 11 * 32},
        {7 * 32, 11 * 32}};

    for (uint8_t i{0}; i < BOX_NUM; ++i)
    {
        BoxObj *box = createObject<BoxObj>();
        box->init();
        box->_x_global = box_arr[i][0];
        box->_y_global = box_arr[i][1];

        _sokoban->addBoxPtr(box);
        _game_objs.push_back(box);
    }
}

void Level1Scene::createBoxPoints()
{
    uint16_t point_arr[BOX_NUM][2] = {
        {14 * 32, 1 * 32},
        {14 * 32, 2 * 32},
        {14 * 32, 3 * 32},
        {14 * 32, 4 * 32},
        {14 * 32, 5 * 32},
        {15 * 32, 1 * 32},
        {15 * 32, 2 * 32},
        {15 * 32, 3 * 32},
        {15 * 32, 4 * 32},
        {15 * 32, 5 * 32},
        {16 * 32, 1 * 32},
        {16 * 32, 2 * 32},
        {16 * 32, 3 * 32},
        {16 * 32, 4 * 32},
        {16 * 32, 5 * 32},
        {17 * 32, 1 * 32},
        {17 * 32, 2 * 32},
        {17 * 32, 3 * 32},
        {17 * 32, 4 * 32},
        {17 * 32, 5 * 32}};

    for (uint8_t i{0}; i < BOX_NUM; ++i)
    {
        BoxPointObj *point = createObject<BoxPointObj>();
        point->init();
        point->_x_global = point_arr[i][0];
        point->_y_global = point_arr[i][1];

        _game_objs.push_back(point);
    }
}
