#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

// TODO Додати меню із списком ігор

using namespace meow;

class GamesScreen : public IScreen
{
public:
    GamesScreen(GraphicsDriver &display);
    virtual ~GamesScreen();

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_W = 1,
    };
};
