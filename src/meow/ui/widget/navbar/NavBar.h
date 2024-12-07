#pragma once
#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{

    class NavBar : public IWidget
    {
    public:
        NavBar(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~NavBar();
        virtual void onDraw() override;
        NavBar *clone(uint16_t id) const override;

        /*!
         * @brief
         *       Задати віджети, які будуть відображатися на панелі навігації.
         *       Не викликайте цей метод для клонованого об'єкту. Виклик призведе до невизначеної поведінки.
         * @param  start
         *       Крайній лівий віджет.
         * @param  middle
         *       Центральний віджет.
         * @param  last
         *       Крайній правий віджет.
         */
        void setWidgets(IWidget *start, IWidget *middle, IWidget *last);

    private:
        IWidget *_first{nullptr};
        IWidget *_middle{nullptr};
        IWidget *_last{nullptr};
    };

}