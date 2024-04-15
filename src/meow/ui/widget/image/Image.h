#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <SD.h>

namespace meow
{

    class Image : public IWidget
    {

    public:
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

        inline void clearTransparentColor()
        {
            _is_transparent = false;
            _is_changed = true;
        }

        // Встановити зображення
        void setSrc(const uint16_t *image);

#ifdef ENABLE_FRAMEBUFFER
        // Якщо увімкнено подвійну беферизацію, необхідно викликати до встановлення зображення.
        // Інакше викликати не потрібно.
        void init(uint16_t width, uint16_t height);
#endif

    private:
        bool _is_transparent{false};
        uint16_t _transparent_color{0xFFFF};

        const uint16_t *_img_ptr{nullptr};
        uint16_t *_psram_img_ptr{nullptr};

#ifdef ENABLE_FRAMEBUFFER
        TFT_eSprite _img_buf = TFT_eSprite(_display.getTFT());
#endif
    };

}