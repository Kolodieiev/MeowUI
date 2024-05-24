#include "Level2Scene.h"
#include "../ui/SceneUI.h"
#include "../map/tile_img/tile_0.h"
#include "../map/template/map_scene_2.h"
#include "../obj/kawaii/KawaiiObj.h"

Level2Scene::Level2Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded) : IGameScene(display, input, stored_objs)
{
    _game_map.addTileDescr(0, Tile::TYPE_GROUND, IMG_TILE_0);
    _game_map.build(9, 8, 32, map_scene_2);
    KawaiiObj *kawaii = createObject<KawaiiObj>();
    kawaii->init();
    kawaii->_x_global = (_display.width() - kawaii->_sprite.width) * 0.5;
    kawaii->_y_global = 0;
    _main_obj = kawaii;
    _game_objs.push_back(kawaii);
    _game_UI = new SceneUI(_display);
}

void Level2Scene::update()
{
    if (_input.isReleased(Input::PIN_START))
    {
        _input.lock(Input::PIN_START, 400);
        _is_finished = true;
        return;
    }

    IGameScene::update();
}

void Level2Scene::onTriggered(int16_t id)
{
}