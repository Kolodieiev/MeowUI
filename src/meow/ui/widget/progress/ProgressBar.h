#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{

    class ProgressBar : public IWidget
    {

    public:
        ProgressBar(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~ProgressBar();
        virtual void onDraw() override;
        virtual ProgressBar *clone(uint16_t id) const override;

        /*!
         * @brief
         *       Задати максимальне значення, яке може бути встановлено в елементі.
         * @param  max
         *       Максимальне значення.
         */
        void setMax(uint16_t max);

        /*!
         * @brief
         *       Задати поточне значення прогресу.
         * @param  progress
         *       Поточне значення прогресу.
         */
        void setProgress(uint16_t progress);

        /*!
         * @brief
         *       Задати колір полоси прогресу.
         * @param  color
         *       Колір полоси прогресу.
         */
        inline void setProgressColor(uint16_t color)
        {
            _progress_color = color;
            _is_changed = true;
        }

        /*!
         * @brief
         *       Задати орієнтацію полоси прогресу.
         *       По замовченню встановлено ORIENTATION_HORIZONTAL.
         * @param  orientation
         *       Може мати значення: ORIENTATION_VERTICAL / ORIENTATION_HORIZONTAL.
         */
        inline void setOrientation(Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }
        inline Orientation getOrientation() const { return _orientation; }

        /*!
         * @brief
         *       Скинути значення прогресу до 1.
         */
        void reset();

        inline uint16_t getProgress() const { return _progress; }
        inline uint16_t getMax() const { return _max; }

    private:
        uint16_t _progress{1};
        uint16_t _max{1};
        uint16_t _progress_color{0xFFFF};
        //
        Orientation _orientation{ORIENTATION_HORIZONTAL};
        // opt
        bool _is_first_draw{true};
        uint16_t _prev_progress{1};
    };

}