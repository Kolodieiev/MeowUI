#pragma once
#include <Arduino.h>
#include "Menu.h"

namespace meow
{

    class FixedMenu : public Menu
    {
    public:
        FixedMenu(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~FixedMenu() {}
        virtual FixedMenu *clone(uint16_t id) const override;

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
    };

}