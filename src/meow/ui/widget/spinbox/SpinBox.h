#pragma once
#include <Arduino.h>
#include "../text/Label.h"

namespace meow
{

    class SpinBox : public Label
    {

    public:
        enum SpinType : uint8_t
        {
            TYPE_INT = 0,
            TYPE_FLOAT
        };

        SpinBox(uint16_t widget_id, GraphicsDriver &display);
        virtual ~SpinBox() {}
        virtual SpinBox *clone(uint16_t id) const override;

        /*!
         * @brief
         *       Збільшити значення, що зберігається в елементі на 1, якщо можливо.
         *
         */
        void up();

        /*!
         * @brief
         *       Зменшити значення, що зберігається в елементі на 1, якщо можливо.
         *
         */
        void down();

        /*!
         * @brief
         *       Задати мінімальне значення, яке може бути встановлено в елементі.
         * @param  min
         *       Мінімальне значення.
         */
        void setMin(float min);
        inline float getMin() const { return _min; }

        /*!
         * @brief
         *       Задати максимальне значення, яке може бути встановлено в елементі.
         * @param  max
         *       Максимальне значення.
         */
        void setMax(float max);
        inline float getMax() const { return _max; }

        /*!
         * @brief
         *       Задати поточне значення для елементу.
         * @param  value
         *       Поточне значення.
         */
        void setValue(float value);
        inline float getValue() const { return _value; }

        /*!
         * @brief
         *       Встановити тип відображення вмісту. По замовченню встановлено TYPE_INT.
         * @param  state
         *
         */
        void setType(SpinType spin_type);
        inline SpinType getType() const { return _spin_type; }

    private:
        using Label::isTicker;
        using Label::setTicker;
        using Label::setText;

        float _min = 0.0f;
        float _max = 0.0f;
        float _value = 0.0f;

        SpinType _spin_type = TYPE_INT;

        void setSpinValToDraw();
    };

}