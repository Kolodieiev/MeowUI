#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <unordered_map>
//
#include "../../driver/graphics/GraphicsDriver.h"
#include "../../driver/audio/wav/WavManager.h"
#include "../../game/ResManager.h"
//
#include "../DataStream.h"
#include "../IdGen.h"
//
#include "../terrain/TerrainManager.h"
//
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

        uint16_t _x_global{0};       // Координата Х відносно ігрового рівня
        uint16_t _y_global{0};       // Координата Y відносно ігрового рівня
        int32_t _x_local{0};         // Координата X відносно дисплея
        int32_t _y_local{0};         // Координата Y відносно дисплея
        SpriteDescription _sprite{}; // Об'єкт структури, яка описує спрайт об'єкта та його стани

        IGameObject(const IGameObject &rhs) = delete;
        IGameObject &operator=(const IGameObject &rhs) = delete;

        IGameObject(GraphicsDriver &display,
                    ResManager &res,
                    WavManager &audio,
                    TerrainManager &terrain,
                    std::unordered_map<uint32_t, IGameObject *> &game_objs);
        virtual ~IGameObject() = 0;

        /**
         * @brief Забезпечує оновлення стану об'єкта. Викликається автоматично ігровим рівнем.
         *
         */
        virtual void update() = 0;

        /**
         * @brief Метод, в якому рекомендується ралізовувати ініціалізацію об'єкта.
         *
         */
        virtual void init() = 0;

        /**
         * @brief Повертає реальний розмір даних об'єкта, які будуть серіалізовані.
         *
         * @return size_t
         */
        virtual size_t getDataSize() const = 0;

        /**
         * @brief Серіалізує об'єкт в DataStream
         *
         * @param ds Об'єкт DataStream, куди буде серіалізовано дані.
         */
        virtual void serialize(DataStream &ds) = 0;

        /**
         * @brief Десеріаізує об'єкт із DataStream
         *
         * @param ds Об'єкт DataStream, з якого будуть прочитані дані у відповідні поля об'єкта.
         */
        virtual void deserialize(DataStream &ds) = 0;

        /**
         * @brief Перемальвоує об'єкт кожен кадр, якщо у нього задано зображення або анімацію.
         *
         */
        virtual void onDraw();

        /**
         * @brief Повертає ідентифікатор типу об'єкта.
         * Необхідний для заміни instanceof.
         *
         * @return int16_t
         */
        int16_t getClassID() const { return _class_ID; }

        /**
         * @brief Повертає ідентифікатор об'єкта.
         *
         * @return uint32_t
         */
        uint32_t getObjId() const { return _obj_id; }

        /**
         * @brief Повертає стан прапора, який вказує чи спрацював тригер у об'єкта.
         *
         * @return true - Якщо об'єкт має тригер і він спрацював. false - Інакше.
         */
        bool isTriggered() const { return _is_triggered; }

        /**
         * @brief Повертає ідентифікатор активного тригера.
         *
         * @return uint8_t
         */
        uint8_t getTriggerID() const { return _trigger_ID; }

        /**
         * @brief Скидає стан тригера об'єкта.
         *
         */
        void resetTrigger() { _is_triggered = false; }

        /**
         * @brief Повертає вказівник на рядок з ім'ям об'єкта, якщо було задано. Інакше на порожій рядок.
         *
         * @return const char*
         */
        const char *getName() const { return _name.c_str(); }

        /**
         * @brief Повертає стан прапору, який вказує чи було об'єкт знищено в попередньому кадрі.
         * Зазвичай використовується сценою для очищення мертвих об'єктів з ігрового рівня.
         * Може бути використаний для запобігання взаємодії з потенційно мертвим об'єктом.
         *
         * @return true - Якщо об'єкт потенційно мертвий. false - Інакше.
         */
        bool isDestroyed() const { return _is_destroyed; }

        /**
         * @brief Get the Layer object
         *
         * @return uint8_t
         */
        uint8_t getLayer() const { return _layer; }

    protected:
        uint32_t _obj_id{0};                                     // Ідентифікатор об'єкта. Може не використовуватися в локальній грі
        uint8_t _class_ID{0};                                    // Ідентифікатор типу об'єкта
        uint8_t _trigger_ID{0};                                  // Ідентифіктор тригера. Може не використовуватися, якщо об'єкт не тригериться.
        bool _is_triggered{false};                               // Прапор спрацювання тригера об'єкта.  Може не використовуватися, якщо об'єкт не тригериться.
        String _name;                                            // Ім'я об'єкта, може не використовуватися
        bool _is_destroyed{false};                               // Прапор знищення об'єкта іншими об'єктами або сценою
        uint8_t _layer{0};                                       // Шар сортування об'єкта по осі Z. Чим більше значення, тим вище шар
        ResManager &_res_manager;                                // Менеджер ресурсів
        WavManager &_audio;                                      // Менеджер аудіо
        TerrainManager &_terrain;                                // Поверхня ігрового рівня
        std::unordered_map<uint32_t, IGameObject *> &_game_objs; // Список ігрових об'єктів на сцені
        GraphicsDriver &_display;

        // Створити спрайт об'єкта. Якщо пам'яті недостатньо, буде викликано перезавантаження esp

        /**
         * @brief Створює спрайт об'єкта.
         * Повинен бути викликаний тільки після заповнення полів об'єкта _sprite, що описує спрайт об'єкта.
         *
         */
        void initSprite();

        /**
         * @brief Перевіряє, чи матиме об'єкт пересічення з будь-яким іншим твердим тілом в заданому напрямку.
         *
         * @param x Координата, де повинна відбутися перевірка.
         * @param y Координата, де повинна відбутися перевірка.
         * @param direction Напрямок руху.
         * @return true - Якщо об'єкт пересікається з іншим твердим тілом в заданих координатах та напрямку. false - Інакше.
         */
        bool hasIntersectWithBody(uint16_t x, uint16_t y, MovingDirection direction);

        /**
         * @brief Повертає всі об'єкти ігрового рівня, що знаходяться в колі з заданим радіусом.
         *
         * @param x_mid Центральна координата кола.
         * @param y_mid Центральна координата кола.
         * @param radius Радіус кола.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjInCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius);

        /**
         * @brief Перевіряє чи пересікається спрайт об'єкта із заданим колом.
         *
         * @param x_mid Центральна координата кола.
         * @param y_mid Центральна координата кола.
         * @param radius Радіус кола.
         * @return true - Якщо спрайт пересікається з колом. false - Інкаше.
         */
        bool hasCollisionWithCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius);

        /**
         * @brief Повертає всі об'єкти ігрового рівня, що знаходяться в заданому прямокутнику.
         *
         * @param x_start Координата верхнього лівого кута прямокутника.
         * @param y_start Координата верхнього лівого кута прямокутника.
         * @param width Ширина прямокутника.
         * @param height Висота прямокутника.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjctsInRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height);

        /**
         * @brief Перевіряє чи пересікається спрайт об'єкта із заданим прямокутником.
         *
         * @param x_start Координата верхнього лівого кута прямокутника.
         * @param y_start Координата верхнього лівого кута прямокутника.
         * @param width Ширина прямокутника.
         * @param height Висота прямокутника.
         * @return true - Якщо спрайт пересікається з прямокутником. false - Інкаше.
         */
        bool hasCollisionWithRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height);

        // Отримати список об'єктів, які перетинаються з точкою
        /**
         * @brief Повертає список об'єктів, спрайти яких перетинацються з заданою точкою.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjInPoint(uint16_t x, uint16_t y);

        /**
         * @brief Перевіряє чи пересікається спрайт об'єкта із заданими координатами.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return true - Якщо спрайт пересікається з координатами. false - Інакше.
         */
        bool hasIntersectWithPoint(uint16_t x, uint16_t y);

        /**
         * @brief Розраховує кут від pivot об'єкта, до вказаної точки.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return uint16_t
         */
        uint16_t calcAngleToPoint(uint16_t x, uint16_t y);

        /**
         * @brief Розраховує відстань від pivot об'єкта до вказаної точки
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return uint16_t
         */
        uint16_t calcDistToPoint(uint16_t x, uint16_t y);

        /**
         * @brief Переміщує об'єкт на вказану кількість пікселів в сторону указаної точки.
         *
         * @param x_to Координата точки.
         * @param y_to Координата точки.
         * @param step_w Розмір кроку в пікселях.
         */
        void stepToPoint(uint16_t x_to, uint16_t y_to, uint16_t step_w);

        /**
         * @brief Створює об'єкт вказаного типу.
         *
         * @tparam T Тип об'єкта, який потрібно створити.
         * @return T* Вказівний на створений об'єкт.
         */
        template <typename T>
        T *createObject()
        {
            try
            {
                return new T(_display, _res_manager, _audio, _terrain, _game_objs);
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