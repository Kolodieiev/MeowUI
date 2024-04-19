#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

//TODO Додати мп3-плеєр

using namespace meow;

class Mp3Screen : public IScreen
{
public:
    Mp3Screen(GraphicsDriver &display);
    virtual ~Mp3Screen();

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_W = 1,
    };
};