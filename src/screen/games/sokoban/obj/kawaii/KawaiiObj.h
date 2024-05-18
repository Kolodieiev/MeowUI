#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

class KawaiiObj : public IGameObject
{

public:
    KawaiiObj(GraphicsDriver &display,
              ResManager &res,
              WavManager &audio,
              GameMap &game_map,
              std::list<IGameObject *> &game_objs) : IGameObject(display, res, audio, game_map, game_objs) {}
    virtual ~KawaiiObj() {}

    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;

private:
    std::vector<const uint16_t *> _anim_dance;
};