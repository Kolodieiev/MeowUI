#pragma once
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{
    class ScreenManager
    {
    public:
        void run();

    private:
        GraphicsDriver _display;
    };

    extern ScreenManager MEOW;

}