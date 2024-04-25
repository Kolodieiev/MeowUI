#pragma once
#include <Arduino.h>

#include "meow/game/IGameScene.h"
#include "../obj/ghost/GhostObj.h"
#include "../obj/sokoban/SokobanObj.h"

using namespace meow;

class Level0Scene : public IGameScene
{

public:
    Level0Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded = false);
    // Деструктор обов'язковий до реалізації.
    virtual ~Level0Scene();

    // Метод обов'язковий до реалізації. Викликається кожен кадр екраном для оновлення та відрисовки сцени.
    virtual void update() override;

    // Метод обов'язковий до реалізації. Його буде викликано, якщо будь-який об'єкт повідомить про свій тригер. В параметр буде передано id тригера
    virtual void onTriggered(int16_t id) override;

private:
    GhostObj *_ghost;     // Об'єкт який дозволяє переміщувати камеру по ігровому рівню.
    SokobanObj *_sokoban; // Вказівник на персонажа. Його не можна видаляти самостійно. Тому що цей об'єкт буде додано до ігрового світу і сцена видалить його сама

    bool _is_ghost_selected{true}; // Прапор, що допомогає розпізнати, який об'єкт зараз обрано головним.

    // Завантажити ігровий рівень
    void buildMap();
    // Створити об'єкт привида
    void createGhost();
    // Створити об'єкт комірника
    void createSokoban();
};
