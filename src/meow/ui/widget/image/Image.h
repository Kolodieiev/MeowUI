#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"
#include <SD.h>

namespace meow
{
    class Image : public IWidget
    {
        // TODO Переписати

    public:
        static const uint16_t COLOR_TRANSPARENT{0xF81F}; // Колір rgb(255, 0, 255)

        Image(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Image();
        virtual void onDraw() override;
        virtual Image *clone(uint16_t id) const override;

        void setTransparentColor(uint16_t color)
        {
            _is_transparent = true;
            _transparent_color = color;
            _is_changed = true;
        }

        void clearTransparency()
        {
            _is_transparent = false;
            _is_changed = true;
        }

        // Встановити зображення із FLASH-пам'яті
        // src не буде видалено разом з віджетом!
        void setSrc(const uint16_t *image);

#ifdef DOUBLE_BUFFERRING
        // Якщо зображення встановлюється з FLASH. Необхідно викликати до його встановлення.
        // При завантаженні зображення з SD, викликати не потрібно.
        void init(uint16_t width, uint16_t height);
#endif

        // Пропустити очищення місця для малювання.
        // Дозволяє використовувати одне і те ж зображення в декількох віджетах.
        void setSkipClear(bool state) { _skip_clear = state; }

    private:
        bool _is_transparent{false};
        bool _skip_clear{false};
        uint16_t _transparent_color{COLOR_TRANSPARENT};

        const uint16_t *_img_ptr{nullptr};
        uint16_t *_psram_img_ptr{nullptr};

#ifdef DOUBLE_BUFFERRING
        TFT_eSprite _img_buf = TFT_eSprite(_display.getTFT());
#endif
    };

}