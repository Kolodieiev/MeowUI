#pragma once
#include <Arduino.h>
#include "./BmpHeader.h"

namespace meow
{

    class BmpUtil
    {

    public:
        const uint16_t *loadBmp(const char *path_to_bmp, BmpHeader &out_header);
        bool saveBmp(BmpHeader &header, const uint16_t *data, const char *path_to_bmp);

    private:
        const uint8_t BMP_HEADER_SIZE{14};
        const uint8_t BMP_INFO_SIZE{40};

        bool validateHeader(const BmpHeader &bmp_header);

        const uint16_t *srcNotFound(BmpHeader &out_header);
    };

}