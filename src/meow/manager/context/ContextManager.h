#pragma once
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{
    class ContextManager
    {
    public:
        void run();

    private:
        GraphicsDriver _display;
    };

    extern ContextManager MEOW;

}