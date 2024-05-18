#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"
#include "../box/BoxObj.h"

using namespace meow;

class SokobanObj : public IGameObject
{

public:
    SokobanObj(GraphicsDriver &display,
               ResManager &res,
               WavManager &audio,
               GameMap &game_map,
               std::list<IGameObject *> &game_objs) : IGameObject(display, res, audio, game_map, game_objs) {}
    virtual ~SokobanObj() {}

    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;
    virtual void onDraw() override; // Необов'язковий метод

    void move(MovingDirection direction);

    // Додати вказівник на новий ящик на сцені. Комірник зможе перевіряти стан ящика по цьому вказівнику.
    void addBoxPtr(BoxObj *box_ptr);

private:
    const uint16_t PIX_PER_STEP{32};

    std::vector<BoxObj *> _boxes;

    // Якщо можливо, переміститися самому в задані координати, та перемістити ящик
    void stepTo(uint16_t x, uint16_t y, uint16_t box_x_step, uint16_t box_y_step);
};
