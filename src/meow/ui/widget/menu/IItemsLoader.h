#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <vector>

namespace meow
{

    class IItemsLoader
    {
    public:
        IItemsLoader(){};
        virtual ~IItemsLoader(){};

        virtual std::vector<IWidget *> loadPrev(uint8_t size, uint16_t current_ID) = 0;
        virtual std::vector<IWidget *> loadNext(uint8_t size, uint16_t current_ID) = 0;
    };

}