#pragma once
#include <Arduino.h>
//
#include "meow/ui/screen/IScreen.h"
#include "meow/game/IGameScene.h"

using namespace meow;

class SokobanScreen : public IScreen
{

public:
    SokobanScreen(GraphicsDriver &display);
    virtual ~SokobanScreen() {}

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    IGameScene *_scene;
    std::vector<IObjShape *> _stored_objs;
};