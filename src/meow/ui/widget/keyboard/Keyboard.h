#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"
#include "./KeyboardRow.h"

namespace meow
{

    class Keyboard : public IWidgetContainer
    {
    public:
        Keyboard(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Keyboard() {}
        virtual void onDraw() override;
        virtual Keyboard *clone(uint16_t id) const override;

        /*!
         * @return Ідентифікатор кнопки, на якій встановлено фокус.
         */
        uint16_t getCurrentBtnID() const;

        /*!
         * @return Текст кнопки, на якій встановлено фокус.
         */
        String getCurrentBtnTxt() const;

        /*!
         * @brief
         *       Якщо можливо підняти фокус на один ряд.
         *       Інакше, якщо можливо, перемістити фокус на самий нижній ряд.
         */
        void focusUp();

        /*!
         * @brief
         *       Якщо можливо опустит фокус на один ряд.
         *       Інакше, якщо можливо, перемістити фокус на самий верхній ряд.
         */
        void focusDown();

        /*!
         * @brief
         *       Якщо можливо перемістити фокус на один елемент вліво.
         *       Інакше, якщо можливо, перемістити фокус на крайній правий елемент в цьому ряду.
         */
        void focusLeft();

        /*!
         * @brief
         *       Якщо можливо перемістити фокус на один елемент вправо.
         *       Інакше, якщо можливо, перемістити фокус на крайній лівий елемент в цьому ряду.
         */
        void focusRight();

    private:
        uint16_t _cur_focus_row_pos{0};

        bool _first_drawing{true};

        KeyboardRow * getFocusRow() const;
    };

}