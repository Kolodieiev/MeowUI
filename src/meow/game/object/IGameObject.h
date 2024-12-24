#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <list>
#include <vector>
//
#include "../../driver/graphics/GraphicsDriver.h"
#include "../../driver/audio/wav/WavManager.h"
#include "../../game/ResManager.h"
//
#include "IObjShape.h"
//
#include "../gmap/GameMap.h"
//
#include "BodyDescription.h"
#include "SpriteDescription.h"

#include "../../driver/audio/wav/WavTrack.h"

namespace meow
{

    class IGameObject
    {
    public:
        enum MovingDirection : uint8_t
        {
            DIRECTION_NONE,
            DIRECTION_UP,
            DIRECTION_DOWN,
            DIRECTION_LEFT,
            DIRECTION_RIGHT
        };
        IGameObject(const IGameObject &rhs) = delete;
        IGameObject &operator=(const IGameObject &rhs) = delete;

        IGameObject(GraphicsDriver &display,
                    ResManager &res,
                    WavManager &audio,
                    GameMap &game_map,
                    std::list<IGameObject *> &game_objs);
        virtual ~IGameObject() = 0;

        // Метод, в якому рекомендується ралізовувати логіку поведінки об'єкта
        virtual void update() = 0;
        // Метод, в якому рекомендується ралізовувати ініціалізацію об'єкта
        virtual void init() = 0;

        // Повертає образ об'єкта, який можна перенести між сценами або зберегти до файлу
        virtual IObjShape *getShape() = 0;

        // Повертає об'єкту властивості із образу
        virtual void reborn(IObjShape *shape) = 0;

        // Службовий метод, що відповідає за відрисовку спрайту об'єкта. Не обов'язковий до перевантаження
        virtual void onDraw();
        //
        uint16_t _x_global{0}; // Координати об'єкта на мапі
        uint16_t _y_global{0}; // Координати об'єкта на мапі
        //
        int32_t _x_local{0}; // Координати об'єкта на екрані
        int32_t _y_local{0}; // Координати об'єкта на екрані
        //
        BodyDescription _body{};     // Структура, що описує характеристики твердого тіла об'єкта
        SpriteDescription _sprite{}; // Структура, яка описує спрайт об'єкта та його стани

        const char *getName() const { return _name; }
        int16_t getTriggerID() const { return _trigger_ID; }
        bool isTriggered() const { return _is_triggered; }
        void resetTrigger() { _is_triggered = false; }
        bool isDestroyed() const { return _is_destroyed; }
        uint8_t getLayer() const { return _layer; }

        // Метод, що дозволяє фільтрувати типи об'єктів. instanceof не підтримується
        int16_t getClassID() const { return _class_ID; }

    protected:
        int16_t _class_ID{-1};     // Ідентифікатор типу об'єкта
        int16_t _trigger_ID{-1};   // Ідентифіктор тригера
        bool _is_triggered{false}; // Прапор спрацювання тригера об'єкта

        const char *_name{nullptr}; // Ім'я об'єкта, може не використовуватися

        bool _is_destroyed{false}; // Прапор знищення об'єкта іншими об'єктами
        uint8_t _layer{0};         // Шар сортування об'єкта по осі Z. Чим більше значення, тим вище шар
        //
        ResManager &_res;                     // Менеджер ресурсів
        WavManager &_audio;                   // Менеджер аудіо
        GameMap &_game_map;                   // ігрова мапа
        std::list<IGameObject *> &_game_objs; // ігрові об'єкти, які достпуні на сцені
        GraphicsDriver &_display;

        // Створити спрайт об'єкта. Якщо пам'яті недостатньо, буде викликано перезавантаження esp
        void initSprite();
        //
        // Повертає true якщо об'єкт пересікається з іншим твердим тілом в заданих координатах
        bool hasIntersectWithBody(uint16_t x, uint16_t y, MovingDirection direction);
        //
        // Вибрати всі об'єкти, що знаходяться в заданому колі.
        std::list<IGameObject *> getObjInCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius);
        // Повертає true якщо спрайт об'єкта пересікається із заданим колом.
        bool hasCollisionWithCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius);
        //
        std::list<IGameObject *> getObjctsInRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height);
        // Повертає true якщо спрайт об'єкта знаходиться у заданому прямокутнику.
        bool hasCollisionWithRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height);
        //
        // Повертає true якщо спрайт об'єкта пересікається з цими координатами
        bool hasIntersectWithPoint(uint16_t x_to, uint16_t y_to);

        // Розрахувати кут від pivot об'єкта, до вказаної точки
        uint16_t calcAngleToPoint(uint16_t x, uint16_t y);

        // Розрахувати відстань від pivot об'єкта до вказаної точки
        uint16_t calcDistToPoint(uint16_t x, uint16_t y);

        // Зробити крок в сторону об'єкта, що знаходиться в указаних координатах з шириною кроку step_w
        void stepToPoint(uint16_t x_to, uint16_t y_to, uint16_t step_w);

        // Отримати список об'єктів, які перетинаються з точкою
        std::list<IGameObject *> getObjInPoint(uint16_t x, uint16_t y);

        //  Метод-шаблон для створення ігрових об'єктів
        template <typename T>
        T *createObject()
        {
            try
            {
                return new T(_display, _res, _audio, _game_map, _game_objs);
            }
            catch (const std::bad_alloc &e)
            {
                log_e("%s", e.what());
                esp_restart();
            }
        }

    private:
        TFT_eSprite _obj_sprite;
    };

}