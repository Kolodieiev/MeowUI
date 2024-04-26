#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

class BoxPointObj : public IGameObject
{

public:
    BoxPointObj(GraphicsDriver &display,
                WavManager &audio,
                GameMap &game_map,
                std::list<IGameObject *> &game_objs) : IGameObject(display, audio, game_map, game_objs) {}
    virtual ~BoxPointObj() {}

    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;
};
