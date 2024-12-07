#pragma once
#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{

    class ToggleSwitch : public IWidget
    {
    public:
        ToggleSwitch(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display) {}
        virtual ~ToggleSwitch(){};
        virtual void onDraw() override;
        virtual ToggleSwitch *clone(uint16_t id) const override;

        /*!
         * @brief
         *      Повертає поточний стан перемикача. false == вимкнений.
         */
        inline bool isOn() const { return _is_on; }

        /*!
         * @brief
         *     Встановити стан перемикача у "Ввімкнений".
         */
        inline void on()
        {
            _is_on = true;
            _is_changed = true;
        }

        /*!
         * @brief
         *     Встановити стан перемикача у "Вимкнений".
         */
        inline void off()
        {
            _is_on = false;
            _is_changed = true;
        }

        /*!
         * @brief
         *     Встановити колір фону важелю.
         */
        inline void setLeverColor(uint16_t color)
        {
            _lever_color = color;
            _is_changed = true;
        }
        inline uint16_t getLeverColor() const { return _lever_color; }

        /*!
         * @brief
         *     Встановити колір фону перемикача у стані "Увімкнений".
         */
        inline void setOnColor(uint16_t color)
        {
            _on_color = color;
            _is_changed = true;
        }
        inline uint16_t getOnColor() const { return _on_color; }

        /*!
         * @brief
         *     Встановити колір фону перемикача у стані "Ввимкнений".
         */
        inline void setOffColor(uint16_t color)
        {
            _off_color = color;
            _is_changed = true;
        }
        inline uint16_t getOffColor() const { return _off_color; }

        /*!
         * @brief
         *     Встановити положення перемикача горизонтальне/вертикальне.
         */
        inline void setOrientation(Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }
        inline Orientation getOrientation() const { return _orientation; }

    private:
        using IWidget::getBackColor;
        using IWidget::setBackColor;

        Orientation _orientation{ORIENTATION_HORIZONTAL};
        bool _is_on{false};
        uint16_t _lever_color{0xFFFF};
        uint16_t _on_color{TFT_GREEN};
        uint16_t _off_color{TFT_RED};
    };

}