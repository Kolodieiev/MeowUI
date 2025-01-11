#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{
    class IWidget
    {

    public:
        enum Alignment : uint8_t
        {
            ALIGN_START = 0,
            ALIGN_CENTER,
            ALIGN_END
        };

        enum Gravity : uint8_t
        {
            GRAVITY_TOP = 0,
            GRAVITY_CENTER,
            GRAVITY_BOTTOM
        };

        enum Orientation : uint8_t
        {
            ORIENTATION_HORIZONTAL = 0,
            ORIENTATION_VERTICAL,
        };

        enum Visibility : uint8_t
        {
            VISIBLE = 0,
            INVISIBLE,
        };

        IWidget(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~IWidget() = 0;
        virtual IWidget *clone(uint16_t id) const = 0;

        virtual void onDraw() = 0;

        void forcedDraw();

        void setPos(uint16_t x, uint16_t y)
        {
            _x_pos = x;
            _y_pos = y;
            _is_changed = true;
        }

        void setHeight(uint16_t height)
        {
            _height = height;
            _is_changed = true;
        }

        void setWidth(uint16_t width)
        {
            _width = width;
            _is_changed = true;
        }

        void setBackColor(uint16_t back_color)
        {
            _back_color = back_color;
            _is_changed = true;
        }

        /*!
         * @brief  Задати батьківський контейнер, відносно якого буде розраховуватись позиціонування.
         *
         */
        void setParent(IWidget *parent)
        {
            _parent = parent;
            _is_changed = true;
        }

        const IWidget *getParent() const { return _parent; }

        /*!
         * @brief  Задати скруглення кутів віджета.
         * @param  radius
         *         Значення скруглення кутів.
         */
        void setCornerRadius(const uint8_t radius)
        {
            _corner_radius = radius;
            _is_changed = true;
        }

        /*!
         * @brief
         *          Встановити відображення межі елементу товщиною 1пкс.
         *          Межа буде відображатися за рахунок ширини віджета.
         * @param  state
         *         Логічне значення стану.
         */
        void setBorder(const bool state)
        {
            _has_border = state;
            _is_changed = true;
        }

        void setBorderColor(uint16_t color)
        {
            _border_color = color;
            _is_changed = true;
        }

        /*!
         * @return
         *        Глобальну X координату віджета.
         *
         */
        uint16_t getXPos() const;

        /*!
         * @return
         *        Глобальну Y координату віджета.
         *
         */
        uint16_t getYPos() const;

        uint8_t getCornerRadius() const { return _corner_radius; }
        uint16_t getID() const { return _id; }
        uint16_t getHeight() const { return _height; }
        uint16_t getWidth() const { return _width; }
        uint16_t getBackColor() const { return _back_color; }
        uint16_t getBorderColor() const { return _border_color; }

        bool hasBorder() const { return _has_border; }

        /*!
         * @brief
         *       Задати флаг відображення межі віджета, при встановленні фокусу на ньому.
         * @param  state
         *       Стан флагу.
         */
        void setChangingBorder(const bool state)
        {
            _need_change_border = state;
            _is_changed = true;
        }

        /*!
         * @brief
         *       Задати флаг зміни кольору фону віджета, при встановленні фокусу на ньому.
         * @param  state
         *       Стан флагу.
         */
        void setChangingBack(const bool state)
        {
            _need_change_back = state;
            _is_changed = true;
        }

        /*!
         * @brief
         *       Задати колір межі віджета, при встановленні фокусу на ньому.
         * @param  color
         *       Колір фону.
         */
        void setFocusBorderColor(uint16_t color)
        {
            _focus_border_color = color;
            _is_changed = true;
        }
        uint16_t getFocusBorderColor() const { return _focus_border_color; }

        /*!
         * @brief
         *       Задати колір фону при встановленні фокусу на віджеті.
         * @param  color
         *       Колір фону.
         */
        void setFocusBackColor(uint16_t color)
        {
            _focus_back_color = color;
            _is_changed = true;
        }
        uint16_t getFocusBackColor() const { return _focus_back_color; }

        /*!
         * @brief Встановити фокус на віджеті.
         */
        void setFocus();

        /*!
         * @brief Прибрати фокус з віджета.
         */
        void removeFocus();

        /*!
         * @brief Встановити видимість віджета.
         */
        void setVisibility(Visibility value)
        {
            _visibility = value;
            _is_changed = true;
        }

        Visibility getVisibility() const { return _visibility; }

        /*!
         * @brief
         *       Визначити, чи має цей віджет перетин з координатами.
         * @param  x
         *       X - координата.
         * @param  y
         *       Y - координата.
         * @return
         *      true у разі, якщо віджет має перетин, інакше false.
         */
        bool hasIntersectWithCoords(uint16_t x, uint16_t y);

        /*!
         * @return
         *      true у разі, якщо віджет являється контейнером для інших віджетів, інакше false.
         */
        bool isContainer() const { return _is_container; }

        /**
         * @brief Встановити прозорість віджета.
         *
         * @param state
         * true - фон віджета повністю прозорий. Необхідно самостійно перемальовувати зображення під віджетом.
         * false - повністю непрозорий.
         */
        void setTransparency(bool state) { _is_transparent = state; }

        /// @return Стан прапору, який вказує, чи є віджет прозорим.
        bool isTransparent() const { return _is_transparent; }

    protected:
        uint16_t _id{0};
        bool _is_container{false};
        bool _is_changed{true};
        bool _has_border{false};
        bool _is_transparent{false};

        uint16_t _x_pos{0};
        uint16_t _y_pos{0};

        uint16_t _width{1};
        uint16_t _height{1};
        uint16_t _back_color{0x0000};
        uint16_t _border_color{0x0000};
        uint8_t _corner_radius{0};
        //
        Visibility _visibility{VISIBLE};
        //
        bool _has_focus{false};
        bool _old_border_state;

        bool _need_change_border{false};
        bool _need_change_back{false};

        uint16_t _focus_border_color{0xFFFF};
        uint16_t _old_border_color{0xFFFF};

        uint16_t _focus_back_color{0xFFFF};
        uint16_t _old_back_color{0xFFFF};
        //
        GraphicsDriver &_display;
        const IWidget *_parent{nullptr};

        /*!
         * @brief  Залити місце розташування віджета фоновим кольором.
         */
        void clear() const;

        /*!
         * @brief  Приховати елемент. Працює, якщо віджет має батьківський контейнер.
         */
        void hide() const;
    };

}