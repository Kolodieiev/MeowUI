#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "Menu.h"
#include "IItemsLoader.h"

namespace meow
{

    class DynamicMenu : public Menu
    {
    public:
        DynamicMenu(IItemsLoader *loader, uint16_t widget_ID, GraphicsDriver &display);
        virtual ~DynamicMenu() {}
        virtual DynamicMenu *clone(uint16_t id) const override;

        /*!
         * @brief
         *       Перемістити фокус на попередній елемент в списку.
         *
         * @return
         *        true в разі успіху операції. Інакше false.
         */
        virtual bool focusUp() override;

        /*!
         * @brief
         *       Перемістити фокус на наступний елемент в списку.
         *
         * @return
         *        true в разі успіху операції. Інакше false.
         */
        virtual bool focusDown() override;

        uint16_t getItemsNumOnScreen() const;

    private:
        IItemsLoader *_loader;
    };

}