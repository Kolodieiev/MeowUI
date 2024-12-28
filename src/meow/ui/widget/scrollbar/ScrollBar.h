#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{

    class ScrollBar : public IWidget
    {
    public:
        ScrollBar(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~ScrollBar() {}
        virtual void onDraw() override;
        virtual ScrollBar *clone(uint16_t id) const override;

        /*!
         * @brief
         *       Підняти слайдер на одну позицію.
         *
         * @return
         *        true в разі успіху операції. Інакше false.
         */
        bool scrollUp();

        /*!
         * @brief
         *       Опустити слайдер на одну позицію.
         *
         * @return
         *        true в разі успіху операції. Інакше false.
         */
        bool scrollDown();

        /*!
         * @brief
         *       Викликати тільки після ініціалізації ширини і висоти полоси прокрутки.
         *       Встановлює кількість позицій полоси прокрутки.
         * @param  max_value
         *       Кількість позицій полоси прокрутки.
         */
        void setMax(uint16_t max_value);

        /*!
         * @brief
         *       Задати поточну позицію слайдеру.
         * @param  value
         *       Поточна позиція слайдера.
         */
        void setValue(uint16_t value);

        /*!
         * @brief
         *       Задати орієнтацію полоси прокрутки.
         *       По замовченню встановлено ORIENTATION_VERTICAL.
         * @param  orientation
         *       Може мати значення: ORIENTATION_VERTICAL / ORIENTATION_HORIZONTAL.
         */
        void setOrientation(Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }

        Orientation getOrientation() const { return _orientation; }

        /*!
         * @brief
         *       Задати колір слайдеру.
         * @param  color
         *       Колір слайдера.
         */
        void setSliderColor(uint16_t color)
        {
            _slider_color = color;
            _is_changed = true;
        }

        uint16_t getValue() const { return _cur_value; }
        uint16_t getMax() const { return _max_value; }

    private:
        using IWidget::isTransparent;
        using IWidget::setTransparency;

        uint16_t _max_value{1};
        uint16_t _cur_value{0};

        uint16_t _slider_color{0xFFFF};

        Orientation _orientation{ORIENTATION_VERTICAL};

        uint16_t _slider_last_x_pos{0};
        uint16_t _slider_last_y_pos{0};

        uint8_t _slider_width{1};
        uint8_t _slider_height{1};
        uint8_t _slider_step_size{1};

        bool _smart_scroll_enabled{false};
        uint16_t _smart_value{0};

        uint8_t _steps_to_scroll{0};
        uint8_t _steps_counter{0};
        uint8_t _scroll_direction{0};
    };

}