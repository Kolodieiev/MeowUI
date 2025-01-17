#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "../../../widget/image/Image.h"
#include "../../../widget/text/Label.h"

namespace meow
{
    class MenuItem : public IWidget
    {
    public:
        MenuItem(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~MenuItem();
        virtual void onDraw() override;
        MenuItem *clone(uint16_t id) const override;

        /**
         * @brief Закріпити Image за елементом списку. Зображення не буде видалено автоматично разом з віджетом. 
         * Одне і те ж зображення може бути закріплене за різними елементами списку.
         * 
         * @param img Вказівник на Image.
         */
        void setIco(Image *img);

        /**
         * @brief Закріпити Label за елементом списку. Label буде видалено автоматично разом з віджетом. 
         * Для кожного елемента списку повинен використовуватися власний Label.
         * 
         * @param lbl Вказівник на Label.
         */
        void setLbl(Label *lbl);
        Label *getLabel() const { return _label; }
        Image *getIco() const { return _ico; }
        String getText() const;

    protected:
        const uint8_t ITEM_PADDING{5};
        using IWidget::setVisibility;

        Image *_ico{nullptr};
        Label *_label{nullptr};
    };
}