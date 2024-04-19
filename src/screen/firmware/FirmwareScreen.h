#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

// TODO Додати інформацію про прошивку
// TODO Додати оновлення прошивки із SD

using namespace meow;

class FirmwareScreen : public IScreen
{
public:
    FirmwareScreen(GraphicsDriver &display);
    virtual ~FirmwareScreen();

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_W = 1,
    };
};