#pragma once
#include <Arduino.h>
#include "./BmpHeader.h"
#include "./src_not_found.h"

namespace meow
{

    class BmpUtil
    {

    public:
        // Завантажити BMP із SD карти. BMP повинен мати глибину кольору 16 біт, а кодування кольору у форматі 565
        const uint16_t *loadBmp(const char *path_to_bmp, BmpHeader &out_header);
        
        // Зберегти зображення до SD карти. Вхідні дані повинні мати глибину кольору 16 біт, а кодування кольору у форматі 565
        bool saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp);

    private:
        bool validateHeader(const BmpHeader &bmp_header);
        const uint16_t *srcNotFound(BmpHeader &out_header);
    };

}