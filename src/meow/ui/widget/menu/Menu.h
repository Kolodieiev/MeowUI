#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"
#include "./item/MenuItem.h"

namespace meow
{

    class Menu : public IWidgetContainer
    {

    public:
        Menu(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Menu() {}
        virtual void onDraw() override;

        virtual bool focusUp() = 0;
        virtual bool focusDown() = 0;

        void deleteWidgets();

        /*!
         * @brief
         *       Знайти елемент в контейнері по його ідентифікатору.
         *
         * @return
         *        Focusable * в разі успіху операції. Інакше nullptr.
         */
        IWidget *findItemByID(uint16_t item_ID) const;

        /*!
         * @brief
         *       Задати висоту для елементів меню.
         *
         * @param  height
         *       Висота елементів меню.
         */
        void setItemHeight(uint16_t height)
        {
            _item_height = height > 0 ? height : 1;
            _is_changed = true;
        }
        uint16_t getItemHeight() const { return _item_height; }

        /*!
         * @brief
         *       Задати ширину для елементів меню.
         *
         * @param  width
         *       Ширина елементів меню.
         */
        void setItemWidth(uint16_t width)
        {
            _item_width = width > 0 ? width : 1;
            _is_changed = true;
        }
        uint16_t getItemsWidth() const { return _item_width; }

        /*!
         * @brief
         *       Задати орієнтацію меню.
         * @param  orientation
         *       Може мати значення: ORIENTATION_VERTICAL / ORIENTATION_HORIZONTAL.
         */
        void setOrientation(const Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }
        Orientation getOrientation() const { return _orientation; }

        /*!
         * @return
         *        Ідентифікатор елементу, на якому встановлено фокус.
         */
        uint16_t getCurrentItemID() const;

        /*!
         * @return
         *        Позиція елементу, на якому встановлено фокус.
         */
        uint16_t getCurrentFocusPos() const { return _cur_focus_pos; };

        /*!
         * @brief
         *       Отримати копію тексту, що зберігається в цьому елементі.
         * @return
         *        Текст елементу, на якому встановлено фокус.
         */
        String getCurrentItemText() const;

        /*!
         * @brief
         *        Отримати вказівник на поточний виділений елемент меню.
         * @return
         *        Вказівник на віджет, якщо розмір меню > 0, nullptr інакше.
         */
        MenuItem *getCurrentItem();

        /*!
         * @brief
         *        Встановити відступи між елементами меню.
         */
        void setItemsSpacing(uint16_t items_spacing) { _items_spacing = items_spacing; }
        uint16_t getItemsSpacing() const { return _items_spacing; }

        bool addItem(MenuItem *item);

    protected:
        uint16_t _first_item_index{0};
        uint16_t _cur_focus_pos{0};

        uint16_t _item_height{2};
        uint16_t _item_width{2};

        uint16_t _items_spacing{0};

        Orientation _orientation{ORIENTATION_HORIZONTAL};

        void drawItems(uint16_t start, uint16_t count);
        uint16_t getCyclesCount() const;

        using IWidgetContainer::addWidget;
        using IWidgetContainer::deleteWidgets;
    };
}