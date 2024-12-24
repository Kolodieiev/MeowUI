#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"
#include "Label.h"

namespace meow
{
    // TODO add multiline

    class TextBox : public Label
    {

    public:
        enum FieldType : uint8_t
        {
            TYPE_TEXT = 0,
            TYPE_PASSWORD
        };

        TextBox(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~TextBox() {}
        virtual TextBox *clone(uint16_t id) const override;
        virtual void onDraw() override;

        /*!
         * @brief Встановити тип текстового поля.
         *
         * @param  type
         *      Тип поля. Може бути TYPE_TEXT, TYPE_PASSWORD.
         */
        void setType(FieldType type) { _type = type; }
        FieldType getType() const { return _type; }

        /*!
         * @brief Видалити останній символ із рядка, що зберігається в цьому віджеті.
         */
        bool removeLastChar();

        /*!
         * @brief Додати символи в кінець рядка текстового поля.
         */
        void addChars(const char *ch);

    private:
        using Label::initWidthToFit;
        using Label::isTicker;
        using Label::setGravity;
        using Label::setTicker;
        using Label::updateWidthToFit;

        FieldType _type = TYPE_TEXT;

        uint16_t getFitStr(String &ret_str) const;
    };

}