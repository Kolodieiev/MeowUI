#pragma once
#include <Arduino.h>
//
#include <list>
//
#include "../driver/graphics/GraphicsDriver.h"
#include "../driver/audio/wav/WavManager.h"
#include "../driver/input/Input.h"
//
#include "./IGameUI.h"
#include "./IGameMenu.h"
#include "./gmap/GameMap.h"
#include "./object/IGameObject.h"
#include "./object/IObjShape.h"

namespace meow
{

    class IGameScene
    {
    public:
        IGameScene(GraphicsDriver &display, Input &input) : _display{display},
                                                            _input{input},
                                                            _game_map{GameMap(display)} {}
        virtual ~IGameScene() = 0;
        virtual void update() = 0;
        virtual void onTriggered(int16_t id) {}
        //
        IGameScene(const IGameScene &rhs) = delete;
        IGameScene &operator=(const IGameScene &rhs) = delete;
        //
        inline bool isFinished() const { return _is_finished; }
        inline bool isReleased() const { return _is_released; }
        inline uint8_t getNextSceneID() const { return _next_scene_ID; }
        //
    protected:
        //
        bool _is_paused{false};
        //
        bool _is_released{false};
        bool _is_finished{false};
        //
        uint8_t _next_scene_ID{0};
        //
        IGameUI *_game_UI{nullptr};
        IGameMenu *_game_menu{nullptr};
        // Об'єкт, за яким слідує камера
        IGameObject *_main_obj{nullptr};
        // Самий нижній шар сцени
        GameMap _game_map;
        // Список усіх ігрових об'єктів на сцені, які повинні взаємодіяти один з одним
        std::list<IGameObject *> _game_objs;
        // Драйвер графіки
        GraphicsDriver &_display;
        // Ввід
        Input &_input;

        WavManager _audio;

        //
        inline void openSceneByID(uint16_t scene_ID)
        {
            _next_scene_ID = scene_ID;
            _is_released = true;
        }
        inline void endGame() { _is_finished = true; }

        template <typename T>
        T *createObject()
        {
            T *t = new T(_display, _audio, _game_map, _game_objs);
            if (!t)
            {
                log_e("Помилка створення об'єкта");
                esp_restart();
            }

            return t;
        }
    };

}