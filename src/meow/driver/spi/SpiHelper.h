#pragma once
#include <Arduino.h>
namespace meow
{

    class SpiHelper
    {

    public:
        volatile static xSemaphoreHandle _mutex;

    private:
        friend class constructor;
        struct constructor
        {
            constructor()
            {
                _mutex = xSemaphoreCreateMutex();
            }
        };

        static constructor _cons;
    };

}
