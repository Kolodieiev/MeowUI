#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <SD.h>

namespace meow
{
    class Image : public IWidget
    {

    public:
        const uint16_t TRANSPARENT_COLOR{0xF81F}; // Колір rgb(255, 0, 255)

        Image(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Image();
        virtual void onDraw();
        virtual Image *clone(uint16_t id) const;

        inline void setTransparentColor(uint16_t color)
        {
            _is_transparent = true;
            _transparent_color = color;
            _is_changed = true;
        }

        inline void clearTransparency()
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

    private:
        bool _is_transparent{false};
        uint16_t _transparent_color{TRANSPARENT_COLOR};

        const uint16_t *_img_ptr{nullptr};
        uint16_t *_psram_img_ptr{nullptr};

#ifdef DOUBLE_BUFFERRING
        TFT_eSprite _img_buf = TFT_eSprite(_display.getTFT());
#endif
    };

}