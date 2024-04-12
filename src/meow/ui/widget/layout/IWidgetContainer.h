#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <vector>

namespace meow
{

    class IWidgetContainer : public IWidget
    {

    public:
        IWidgetContainer(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~IWidgetContainer();

        /*!
         * @brief
         *       Додати новий віджет до контейнера.
         *       Ідентифікатор нового віджета повинен бути унікальним для цього контейнера.
         * @param widget_ptr
         *       Вказівник на новий віджет.
         * @return
         *        true у разі успіху операції. Інакше false.
         */
        virtual bool addWidget(IWidget *widget_ptr);

        /*!
         * @brief
         *       Видалити віджет по його ідентифікатору з контейнера.
         * @param widget_ID
         *       Ідентифікатор віджета.
         * @return
         *        true у разі успіху операції. Інакше false.
         */
        virtual bool deleteWidgetByID(uint16_t widget_ID);

        /*!
         * @brief
         *       Знайти віджет по його ідентифікатору у контейнері.
         * @param widget_ID
         *       Ідентифікатор віджета.
         * @return
         *        Вказівник на віджет у разі успіху. Інакше nullptr.
         */
        virtual IWidget *findWidgetByID(uint16_t widget_ID) const;

        /*!
         * @brief
         *       Отримати віджет по його порядковому номері у контейнері.
         * @param widget_pos
         *       Порядковий номер віджета.
         * @return
         *       Вказівник на віджет у разі успіху. Інакше nullptr.
         */
        virtual IWidget *getWidgetByPos(uint16_t widget_pos) const;

        /*!
         * @brief
         *       Видалити усі елементи з контейнера та очистити пам'ять, яку вони займали.
         */
        virtual void deleteWidgets();

        /*!
         * @return
         *        Кількість елементів у контейнері.
         */
        inline uint16_t getSize() const { return _widgets.size(); };

        /*!
         * @brief
         *      Розблоковує перерисовку віджетів, що знаходяться всередині цього контейнера.
         */
        inline void enable()
        {
            _is_enabled = true;
        }

        /*!
         * @brief
         *        Блокує перерисовку віджетів, що знаходяться всередині цього контейнера.
         */
        inline void disable()
        {
            _is_enabled = false;
        }

        inline bool isEnabled() const { return _is_enabled; }

    protected:
        bool _is_enabled{true};
        std::vector<IWidget *> _widgets;
    };

}