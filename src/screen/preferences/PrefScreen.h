#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"


using namespace meow;

class PrefScreen : public IScreen
{
public:
    PrefScreen(GraphicsDriver &display);
    virtual ~PrefScreen();

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_W = 1,
    };
};