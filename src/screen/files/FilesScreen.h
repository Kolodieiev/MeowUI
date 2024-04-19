#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

// TODO файловий менеджер

using namespace meow;

class FilesScreen : public IScreen
{
public:
    FilesScreen(GraphicsDriver &display);
    virtual ~FilesScreen();

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_W = 1,
    };
};