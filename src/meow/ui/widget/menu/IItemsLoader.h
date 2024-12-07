#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "./item/MenuItem.h"
#include <vector>

namespace meow
{

    class IItemsLoader
    {
    public:
        IItemsLoader(){};
        virtual ~IItemsLoader(){};

        virtual std::vector<MenuItem *> loadPrev(uint8_t size, uint16_t current_ID) = 0;
        virtual std::vector<MenuItem *> loadNext(uint8_t size, uint16_t current_ID) = 0;
    };

}