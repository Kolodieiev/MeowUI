#pragma once
#include <Arduino.h>

#include "meow/game/IGameScene.h"

using namespace meow;

class Level2Scene : public IGameScene
{

public:
    Level2Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded = false);
    virtual ~Level2Scene() {}
    virtual void update() override;
    virtual void onTriggered(int16_t id) override;
};
