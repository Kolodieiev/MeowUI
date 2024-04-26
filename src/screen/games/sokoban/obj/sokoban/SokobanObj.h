#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

class SokobanObj : public IGameObject
{

public:
    SokobanObj(GraphicsDriver &display,
               WavManager &audio,
               GameMap &game_map,
               std::list<IGameObject *> &game_objs) : IGameObject(display, audio, game_map, game_objs) {}
    virtual ~SokobanObj() {}

    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;
    virtual void onDraw() override; // Необов'язковий метод

    void move(MovingDirection direction);

private:
    const uint16_t PIX_PER_STEP{32};
};
