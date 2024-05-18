#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

class GhostObj : public IGameObject
{

public:
    GhostObj(GraphicsDriver &display,
             ResManager &res,
             WavManager &audio,
             GameMap &game_map,
             std::list<IGameObject *> &game_objs) : IGameObject(display, res, audio, game_map, game_objs) {}
    virtual ~GhostObj() {}
    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;

    void move(MovingDirection direction);

private:
    const uint16_t PIX_PER_STEP{20}; // Кількість пікселів пройдених за кадр
};
