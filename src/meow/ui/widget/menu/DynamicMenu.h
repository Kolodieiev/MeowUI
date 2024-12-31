#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "Menu.h"

namespace meow
{
    typedef std::function<void(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)> OnNextItemsLoad;
    typedef std::function<void(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)> OnPrevItemsLoad;

    class DynamicMenu : public Menu
    {
    public:
        DynamicMenu(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~DynamicMenu() {}
        virtual DynamicMenu *clone(uint16_t id) const override;

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

        /**
         * @brief Розраховує кількість елементів, що може відобразити меню.
         *
         * @return Кількість елементів, які може відобразити меню за поточних розмірів.
         */
        uint16_t getItemsNumOnScreen() const;

        /**
         * @brief Встановити обробник, який завантажить наступні елементи для меню.
         *
         * @param handler Обробник.
         * @param arg Аргументи.
         */
        void setOnNextItemsLoadHandler(OnNextItemsLoad handler, void *arg)
        {
            _next_items_load_handler = handler;
            _next_items_load_arg = arg;
        }

        /**
         * @brief Встановити обробник, який завантажить попередні елементи для меню.
         *
         * @param handler Обробник.
         * @param arg Аргументи.
         */
        void setOnPrevItemsLoadHandler(OnPrevItemsLoad handler, void *arg)
        {
            _prev_items_load_handler = handler;
            _prev_items_load_arg = arg;
        }

    private:
        OnNextItemsLoad _next_items_load_handler{nullptr};
        void *_next_items_load_arg{nullptr};
        //
        OnPrevItemsLoad _prev_items_load_handler{nullptr};
        void *_prev_items_load_arg{nullptr};
    };

}