#pragma once
#include <TFT_eSPI.h>
#include "./driver/graphics/GraphicsDriver.h"

namespace meow
{

    class Meow
    {
    public:
        void run();

    private:
        GraphicsDriver _display;
    };

    extern Meow MEOW;

}