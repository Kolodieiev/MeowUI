#pragma GCC optimize("O3")

#include "Image.h"
#include "../../../util/bmp/BmpUtil.h"

namespace meow
{
    Image::Image(uint16_t wiget_ID, GraphicsDriver &display) : IWidget(wiget_ID, display) {}

#ifdef DOUBLE_BUFFERRING

    Image *Image::clone(uint16_t id) const
    {
        try
        {
            Image *clone = new Image(id, _display);

            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;

            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_is_transparent = _is_transparent;
            clone->_img_ptr = _img_ptr;
            clone->_img_buf = TFT_eSprite(_display.getTFT());
            clone->_img_buf.setSwapBytes(true);
            clone->_img_buf.setColorDepth(16);
            clone->_img_buf.setAttribute(PSRAM_ENABLE, true);
            clone->_img_buf.createSprite(_width, _height, 1);
            clone->_has_transp_color = _has_transp_color;
            clone->_transparent_color = _transparent_color;

            if (!clone->_img_buf.getPointer())
            {
                log_e("Помилка створення спрайту зображення");
                esp_restart();
            }

            clone->setSrc(_img_ptr);

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void Image::setSrc(const uint16_t *image)
    {
        if (_img_buf.getPointer() == nullptr)
        {
            log_e("Буфер не було ініціалізовано");
            esp_restart();
        }

        _is_changed = true;
        _img_ptr = image;
        _img_buf.pushImage(0, 0, _width, _height, _img_ptr);
    }

    void Image::init(uint16_t width, uint16_t height)
    {
        _img_buf.deleteSprite();

        _width = width;
        _height = height;

        _img_buf.setSwapBytes(true);
        _img_buf.setColorDepth(16);
        _img_buf.setAttribute(PSRAM_ENABLE, true);
        _img_buf.createSprite(_width, _height, 1);

        if (_img_buf.getPointer() == nullptr)
        {
            log_e("Помилка ініціалізації буферу %u на %u", _width, _height);
            esp_restart();
        }
    }

    Image::~Image()
    {
        _img_buf.deleteSprite();
    }

    void Image::onDraw()
    {
        if (!_is_changed)
            return;

        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            if (!_is_transparent)
                hide();
            return;
        }

        if (!_is_transparent)
            clear();

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent != nullptr)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (_img_ptr != nullptr)
        {
            if (!_has_transp_color)
                _display.pushSprite(_img_buf, _x_pos + x_offset, _y_pos + y_offset);
            else
                _display.pushSprite(_img_buf, _x_pos + x_offset, _y_pos + y_offset, _transparent_color);
        }
        else
        {
            log_e("Не встановлено src");
            esp_restart();
        }
    }

#else
    Image::~Image() {}

    Image *Image::clone(uint16_t id) const
    {
        try
        {
            Image *clone = new Image(*this);
            clone->_id = id;
            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void Image::setSrc(const uint16_t *image)
    {
        _img_ptr = image;
        _is_changed = true;
    }

    void Image::onDraw()
    {
        if (!_is_changed)
            return;

        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            if (!_is_transparent)
                hide();
            return;
        }

        if (!_is_transparent)
            clear();

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent != nullptr)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (_img_ptr != nullptr)
        {
            if (!_has_transp_color)
                _display.pushImage(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _img_ptr);
            else
                _display.pushImage(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _img_ptr, _transparent_color);
        }
        else
        {
            log_e("Не встановлено src");
            esp_restart();
        }
    }
#endif
}