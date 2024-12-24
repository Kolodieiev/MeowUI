#include "IGameScene.h"
#pragma GCC optimize("O3")
#include <algorithm>

namespace meow
{
    IGameScene::IGameScene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs) : _display{display},
                                                                                                           _input{input},
                                                                                                           _game_map{GameMap(display)},
                                                                                                           _stored_objs{stored_objs}
    {
        _obj_mutex = xSemaphoreCreateMutex();

        if (!_obj_mutex)
        {
            log_e("Не вдалося створити _obj_mutex");
            esp_restart();
        }
    }

    IGameScene::~IGameScene()
    {
        for (auto &&it : _game_objs)
            delete it;

        delete _game_UI;
        delete _game_menu;
    }

    void IGameScene::update()
    {
        if (_is_paused)
            return;

        takeLock();

        _game_map.setCameraPos(_main_obj->_x_global, _main_obj->_y_global);
        _game_map.onDraw();

        std::list<IGameObject *> view_obj;
        IGameObject *obj;

        for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
        {
            obj = *it;

            if (!obj->isDestroyed())
            {

                obj->update();

                if (obj->isTriggered())
                {
                    obj->resetTrigger();
                    onTrigger(obj->getTriggerID());
                }

                if (_game_map.isInView(obj->_x_global, obj->_y_global, obj->_sprite.width, obj->_sprite.height))
                {
                    if (obj != _main_obj)
                    {
                        obj->_x_local = obj->_x_global - _game_map._view_x;
                        obj->_y_local = obj->_y_global - _game_map._view_y;
                    }
                    else
                    {
                        uint16_t half_view_w = (float)_game_map.VIEW_W * 0.5;
                        uint16_t half_view_h = (float)_game_map.VIEW_H * 0.5;

                        if (_main_obj->_x_global < half_view_w)
                            _main_obj->_x_local = _main_obj->_x_global;
                        else if (_main_obj->_x_global < _game_map._map_w - half_view_w)
                            _main_obj->_x_local = half_view_w;
                        else
                            _main_obj->_x_local = _game_map.VIEW_W + _main_obj->_x_global - _game_map._map_w;

                        if (_main_obj->_y_global < half_view_h)
                            _main_obj->_y_local = _main_obj->_y_global;
                        else if (_main_obj->_y_global < _game_map._map_h - half_view_h)
                            _main_obj->_y_local = half_view_h;
                        else
                            _main_obj->_y_local = _game_map.VIEW_H + _main_obj->_y_global - _game_map._map_h;
                    }
                    view_obj.push_back(obj);
                }
            }
            else
            {
                delete (*it);
                _game_objs.erase(it);
            }
        }

        view_obj.sort([](IGameObject *a, IGameObject *b)
                      { if(a->getLayer() < b->getLayer())
                        return true;
                    else
                        return a->_y_global + a->_sprite.height < b->_y_global + b->_sprite.height; });

        for (auto it = view_obj.begin(), last_it = view_obj.end(); it != last_it; ++it)
            (*it)->onDraw();

        giveLock();

        _game_UI->onDraw();
    }

}