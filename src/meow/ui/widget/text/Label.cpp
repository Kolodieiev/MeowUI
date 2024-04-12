#include "Label.h"
namespace meow
{

    Label::Label(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display)
    {
        _height = chr_hgt_font2;
    }

    Label *Label::clone(uint16_t id) const
    {
        Label *clone = new Label(*this);

        if (!clone)
        {
            log_e("Помилка клонування");
            esp_restart();
        }

        clone->_id = id;
        return clone;
    }

    void Label::initWidth(uint16_t add_width_value)
    {
        _width = calcTextPixels();
        _width += 2 + add_width_value + _text_offset;

        _is_changed = true;
    }

    void Label::setWidthToFit(uint16_t add_width_value)
    {
        _temp_width = calcTextPixels();
        _temp_width += 2 + add_width_value + _text_offset;

        _is_changed = true;
    }

    void Label::setText(const char *text)
    {
        _text = text;
        _text_len = getRealStrLen(_text);
        _first_draw_char_pos = 0;

        _dynamic_text.clear();
        _is_dynamic = false;
        _is_changed = true;
    }

    void Label::setText(String text)
    {
        _dynamic_text = text;
        _text_len = getRealStrLen(_dynamic_text);
        _first_draw_char_pos = 0;

        _is_dynamic = true;
        _text = NULL;
        _is_changed = true;
    }

    const char *Label::getText() const
    {
        if (!_is_dynamic)
        {
            if (_text == nullptr)
                return "";
            else
                return _text;
        }
        else
            return _dynamic_text.c_str();
    }

    uint8_t Label::getCharHgt() const
    {
        if (_font_ID == 2)
            return chr_hgt_font2 * _text_size;
        else
            return chr_hgt_font4 * _text_size;
    }

    void Label::setMultiline(bool state)
    {
        _is_multiline = state;
        _is_changed = true;
    }

    void Label::setTicker(bool state)
    {
        _is_ticker = state;
        _temp_is_ticker = state;

        if (!state)
            _first_draw_char_pos = 0;

        _is_changed = true;
    }

    void Label::setTickerInFocus(bool state)
    {
        _is_ticker_in_focus = state;
        _temp_is_ticker_in_focus = state;

        if (!state && _is_ticker)
            _first_draw_char_pos = 0;

        _is_changed = true;
    }

    void Label::setFontID(uint8_t font_ID)
    {
        if (font_ID != 2 && font_ID != 4)
            _font_ID = 2;
        else
            _font_ID = font_ID;

        _height = _font_ID == 2 ? chr_hgt_font2 * _text_size : chr_hgt_font4 * _text_size; // інші шрифти не підтримуються

        _is_changed = true;
    }

    uint16_t Label::calcXStrOffset(uint16_t str_pix_num) const
    {
        if (_width > str_pix_num + _text_offset)
        {
            if (_is_ticker)
                return _text_offset;

            switch (_text_alignment)
            {
            case ALIGN_START:
                return _text_offset;

            case ALIGN_CENTER:
                return (float)(_width - str_pix_num) / 2 + _text_offset;

            case ALIGN_END:
                return _width - str_pix_num + _text_offset;
            }
        }

        return 0;
    }

    uint16_t Label::calcYStrOffset() const
    {
        uint8_t char_height{0};

        if (_font_ID == 2)
            char_height = chr_hgt_font2 * _text_size;
        else if (_font_ID == 4)
            char_height = chr_hgt_font4 * _text_size;

        if (_height > char_height)
        {
            switch (_text_gravity)
            {
            case GRAVITY_TOP:
                return 0;

            case GRAVITY_CENTER:
                return (float)(_height - char_height) / 2;

            case GRAVITY_BOTTOM:
                return _height - char_height;
            }
        }

        return 0;
    }

    uint16_t Label::getRealStrLen(const String &str) const
    {
        if (str == nullptr || str == "")
            return 0;

        uint16_t length{0};

        for (char c : str)
        {
            if ((c & 0xC0) != 0x80)
            {
                ++length;
            }
        }
        return length;
    }

    uint16_t Label::calcTextPixels(uint16_t char_pos) const
    {
        const char *ch_str = _is_dynamic ? _dynamic_text.c_str() : _text;

        if (ch_str == nullptr || strcmp(ch_str, "") == 0)
            return 0;

        uint16_t len = strlen(ch_str);
        uint16_t n = char_pos;

        uint16_t unicode;
        uint16_t pix_sum{0};

        if (_font_ID == 2)
            while (n < len)
            {
                unicode = _display.decodeUTF8((uint8_t *)ch_str, &n, len - n);
                unicode = getCharPos(unicode);
                pix_sum += pgm_read_byte(widtbl_f2 + unicode);
            }
        else
            while (n < len)
            {
                unicode = _display.decodeUTF8((uint8_t *)ch_str, &n, len - n);
                unicode = getCharPos(unicode);
                pix_sum += pgm_read_byte(widtbl_f4 + unicode);
            }

        return pix_sum * _text_size;
    }

    uint16_t Label::getFitStr(String &ret_str, uint16_t start_pos) const
    {
        uint16_t chars_counter{0};

        const char *ch_str = _is_dynamic ? _dynamic_text.c_str() : _text;

        if (ch_str == nullptr || strcmp(ch_str, "") == 0)
            return 0;

        uint16_t len = strlen(ch_str);

        if (start_pos >= len)
        {
            log_e("Стартова позиція повинна бути меншою за довжину рядка");
            esp_restart();
        }

        uint16_t n = start_pos;

        uint16_t unicode;
        uint16_t pix_sum{0};

        if (_font_ID == 2)
        {
            while (n < len)
            {
                unicode = _display.decodeUTF8((uint8_t *)ch_str, &n, len - n);
                unicode = getCharPos(unicode);

                uint16_t char_w = pgm_read_byte(widtbl_f2 + unicode) * _text_size;

                if (pix_sum + char_w >= _width - _text_offset - 2)
                    break;
                else
                {
                    pix_sum += char_w;
                    chars_counter++;
                }
            }
        }
        else if (_font_ID == 4)
        {
            while (n < len)
            {
                unicode = _display.decodeUTF8((uint8_t *)ch_str, &n, len - n);
                unicode = getCharPos(unicode);

                uint16_t char_w = pgm_read_byte(widtbl_f4 + unicode) * _text_size;

                if (pix_sum + char_w >= _width - _text_offset - 2)
                    break;
                else
                {
                    pix_sum += char_w;
                    chars_counter++;
                }
            }
        }

        ret_str = getSubStr(ch_str, start_pos, chars_counter);

        return pix_sum;
    }

    String Label::getSubStr(const String &str, uint16_t start, uint16_t length) const
    {
        if (!length)
            return "";

        unsigned int c, i, ix, q;

        unsigned int min = -1, max = -1; //

        for (q = 0, i = 0, ix = str.length(); i < ix; ++i, q++)
        {
            if (q == start)
                min = i;

            if (q <= start + length || length == -1)
                max = i;

            c = (unsigned char)str[i];

            if (c >= 0 && c <= 127)
                i += 0;
            else if ((c & 0xE0) == 0xC0)
                i += 1;
            else if ((c & 0xF0) == 0xE0)
                i += 2;
            else if ((c & 0xF8) == 0xF0)
                i += 3;
            else
                return ""; // invalid utf8
        }

        if (q <= start + length || length == -1)
            max = i;

        if (min == -1 || max == -1)
            return "";

        return str.substring(min, max);
    }

    uint16_t Label::getCharPos(uint16_t unicode) const
    {
        if (unicode > 127)
        {
            if (unicode > 1039 && unicode < 1104)
                unicode = unicode - 944;
            else
                switch (unicode)
                {
                case 1030:
                    unicode = 41;
                    break;
                case 1031:
                    unicode = 162;
                    break;
                case 1111:
                    unicode = 163;
                    break;
                case 1110:
                    unicode = 73;
                    break;
                case 1028:
                    unicode = 160;
                    break;
                case 1108:
                    unicode = 161;
                    break;
                case 1025:
                    unicode = 37;
                    break;
                case 1105:
                    unicode = 69;
                    break;
                case 1168:
                    unicode = 164;
                    break;
                case 1169:
                    unicode = 165;
                    break;
                case 171:
                case 187:
                    unicode = 2;
                    break;
                default:
                    unicode = CHRS_NMBR - 1;
                }
        }
        else
            unicode -= 32;

        return unicode;
    }

    void Label::onDraw()
    {
        if (!_is_changed)
        {
            if (!_is_ticker && !(_is_ticker_in_focus && _has_focus) || _visibility == INVISIBLE)
                return;
            else if ((millis() - _last_time_ticker_update) < TICKER_UPDATE_DELAY)
                return;
            else
                _last_time_ticker_update = millis();
        }
        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            hide();
            return;
        }

        if (_temp_is_ticker_in_focus && !_has_focus)
            _first_draw_char_pos = 0;

        clear();

        if (_temp_width != 0)
        {
            _width = _temp_width;
            _temp_width = 0;
        }

        _display.setTextFont(_font_ID);
        _display.setTextSize(_text_size);
        _display.setTextColor(_text_color);

        uint16_t txtYPos = calcYStrOffset();
        uint16_t str_pix_num = calcTextPixels();

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent != nullptr)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (_back_img != nullptr)
        {
            _back_img->setPos(_x_pos + x_offset, _y_pos + y_offset);
            _back_img->onDraw();
        }

        if (str_pix_num + _text_offset < _width)
        {
            // Якщо текст не потребує прокрутки, вимикаємо її для зменшення навантаження на МК.
            _is_ticker = false;
            _is_ticker_in_focus = false;

            uint16_t txt_x_pos = calcXStrOffset(str_pix_num);

            if (!_is_dynamic)
            {
                if (_text != nullptr)
                    _display.drawString(_text, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
            else
                _display.drawString(_dynamic_text, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
        }
        else
        {
            String sub_str;
            uint16_t sub_str_pix_num{0};
            uint16_t txt_x_pos{0};

            if (!_is_multiline)
            {
                sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);
                txt_x_pos = calcXStrOffset(sub_str_pix_num);

                if (_temp_is_ticker || (_temp_is_ticker_in_focus && _has_focus))
                {
                    if (_first_draw_char_pos == _text_len - 1 || sub_str_pix_num == 0)
                        _first_draw_char_pos = 0;
                    else
                        _first_draw_char_pos++;
                }

                _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
            else
            {
                uint8_t font_height = _font_ID == 2 ? chr_hgt_font2 : chr_hgt_font4;
                uint8_t line_spacing = (float)font_height * _text_size * 0.15;

                _first_draw_char_pos = 0;
                _is_ticker = false;
                _is_ticker_in_focus = false;

                if (font_height * 2 + line_spacing < _height - 2) // Якщо можна помістити більше ніж один рядок тексту
                {
                    uint16_t y_pos{0};

                    while (y_pos + font_height + line_spacing < _height - 2)
                    {
                        sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);

                        if (sub_str_pix_num > 0)
                        {
                            txt_x_pos = calcXStrOffset(sub_str_pix_num);

                            _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + y_pos);

                            y_pos += font_height + line_spacing;
                            _first_draw_char_pos += getRealStrLen(sub_str);
                        }
                        else
                            break;
                    }
                }
                else
                {
                    sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);
                    txt_x_pos = calcXStrOffset(sub_str_pix_num);
                    _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
                }
            }
        }
    }

}