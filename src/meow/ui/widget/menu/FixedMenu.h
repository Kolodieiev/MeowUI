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

        /*!
         * @brief
         *       Встановити прапор зациклювання.
         */
        void setLoopState(bool state) { _is_loop_enbl = state; }

        /**
         * @brief Встановити фокус за вказаним номером елемента. Якщо елемент на вказаній позиції відсутній, поточний фокус не буде змінено.
         *
         * @param focus_pos порядковий номер елемента в цьому меню.
         */
        void setCurrentFocusPos(uint16_t focus_pos);

    protected:
        bool _is_loop_enbl = false;
    };

}