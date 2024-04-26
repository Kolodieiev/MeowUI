#include "Level0Scene.h"

#include "../obj/TriggerID.h"

// Підключити шаблон мапи рівня
#include "../map/template/map_scene_0.h"

// Додати зображення плиток
#include "../map/tile_img/tile_0.h"
#include "../map/tile_img/tile_1.h"
//
#include "../ui/SceneUI.h"
// Ігрові об'єкти
#include "../obj/box/BoxObj.h"
#include "../obj/box_point/BoxPointObj.h"

Level0Scene::Level0Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded)
    : IGameScene(display, input, stored_objs)
{

    buildMap();

    createGhost();

    createSokoban();

    createBoxes();

    createBoxPoints();

    _game_UI = new SceneUI(_display); // Встановити шар UI. Тут можуть відрисовуватися різноманітні інформаційні панелі
}

Level0Scene::~Level0Scene()
{
    delete _ghost; // Цей об'єкт необхідно видаляти самостійно, тому що він не додається до ігрового світу
}

void Level0Scene::update()
{
    IGameObject::MovingDirection direction = IGameObject::DIRECTION_NONE;

    if (_input.isReleased(Input::PIN_START))
    {
        _input.lock(Input::PIN_START, 400);
        _input.reset();
        _is_finished = true;
        return;
    }

    if (_input.isReleased(Input::PIN_D))
    {
        _input.lock(Input::PIN_D, 400);
        _is_ghost_selected = !_is_ghost_selected;
        _input.reset(); // Скинути необроблені натискання
    }

    if (_is_ghost_selected)
    {
        _main_obj = _ghost; // Якщо обрано привида встановлюємо його вказівник головним у сцені

        if (_input.isHolded(Input::PIN_UP)) // Оброблюємо тільки утримання кнопок
            direction = IGameObject::DIRECTION_UP;
        else if (_input.isHolded(Input::PIN_DOWN))
            direction = IGameObject::DIRECTION_DOWN;
        else if (_input.isHolded(Input::PIN_RIGHT))
            direction = IGameObject::DIRECTION_RIGHT;
        else if (_input.isHolded(Input::PIN_LEFT))
            direction = IGameObject::DIRECTION_LEFT;

        if (direction != IGameObject::DIRECTION_NONE) // Якщо натиснута кнопка переміщеня
            _ghost->move(direction);
    }
    else
    {
        _main_obj = _sokoban; // Встановлюємо головним об'єктом комірника

        if (_input.isReleased(Input::PIN_UP)) // Оброблюємо тільки відпусканя кнопок
        {
            _input.lock(Input::PIN_UP, 100); // Блокуємо кнопку на 200мс, щоб запобігти випадковим зайвим натисканням
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

    IGameScene::update(); // Необхідно обов'язково перевикликати метод кожен кадр у базовму класі. Інакше сцена не буде перемальовуватися.
}

void Level0Scene::onTriggered(int16_t id)
{
    if (id == TriggerID::TRIGGER_NEXT_SCENE)
        _is_finished = true;
}

void Level0Scene::buildMap()
{
    // Створити опис кожного спрайта мапи.
    _game_map.addTileDescr(0, Tile::TYPE_GROUND, IMG_TILE_0);
    _game_map.addTileDescr(1, Tile::TYPE_WALL, IMG_TILE_1);

    uint8_t tiles_x_num{9}; // Кількість плиток мапи по ширині
    uint8_t tiles_y_num{9}; // Кількість плиток мапи по висоті
    uint8_t tile_width{32}; // Ширина або висота плитки. Плитки повинні бути квадратними

    _game_map.build(tiles_x_num, tiles_y_num, tile_width, map_scene_0); // Побудувати мапу на основі даних шаблону
}

void Level0Scene::createGhost()
{
    _ghost = createObject<GhostObj>(); // Створити об'єкт привида
    _ghost->init();                    // Обов'язковий виклик для ініціалізації об'єкта
    _main_obj = _ghost;                // Встановити головним об'єктом сцени привида. Саме за цим головним об'єктом слідкує viewport
}

void Level0Scene::createSokoban()
{
    _sokoban = createObject<SokobanObj>(); // Створити об'єкт комірника
    _sokoban->init();
    _game_objs.push_back(_sokoban); // Додати об'єкт до ігрового світу
    _sokoban->_x_global = 2 * 32;   // Встановити глобальні координати об'єкта на 3й плитці зліва.
                                    // Точка відліку починається у верхньому лівому куті як для мапи так і для об'єктів.
    _sokoban->_y_global = 2 * 32;   // Встановити глобальні координати об'єкта на 3й плитці зверху.
}

void Level0Scene::createBoxes()
{
    // Координати усіх ящиків
    uint16_t box_arr[BOX_NUM][2] = {
        {3 * 32, 2 * 32},
        {4 * 32, 3 * 32},
        {4 * 32, 4 * 32},
        {1 * 32, 6 * 32},
        {3 * 32, 6 * 32},
        {4 * 32, 6 * 32},
        {5 * 32, 6 * 32},
    };

    for (uint8_t i{0}; i < BOX_NUM; ++i)
    {
        BoxObj *box = createObject<BoxObj>(); // Створити об'єкт ящика
        box->init();
        box->_x_global = box_arr[i][0];
        box->_y_global = box_arr[i][1];

        _sokoban->addBoxPtr(box);  // Додати вказівник на об'єкт до комірника.
        _game_objs.push_back(box); // Додати об'єкт до ігрового світу
    }
}

void Level0Scene::createBoxPoints()
{
    // Координати усіх точок
    uint16_t point_arr[BOX_NUM][2] = {
        {1 * 32, 2 * 32},
        {5 * 32, 3 * 32},
        {4 * 32, 5 * 32},
        {1 * 32, 4 * 32},
        {3 * 32, 6 * 32},
        {6 * 32, 6 * 32},
        {4 * 32, 7 * 32},
    };

    for (uint8_t i{0}; i < BOX_NUM; ++i)
    {
        BoxPointObj *point = createObject<BoxPointObj>(); // Створити об'єкт точки
        point->init();
        point->_x_global = point_arr[i][0];
        point->_y_global = point_arr[i][1];

        _game_objs.push_back(point);
    }
}
