#pragma once
#include <Arduino.h>
#include "./BmpHeader.h"
#include "./src_not_found.h"

namespace meow
{

    class BmpUtil
    {

    public:
        const uint16_t *loadBmp(const char *path_to_bmp, BmpHeader &out_header);
        bool saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp);

    private:
        bool validateHeader(const BmpHeader &bmp_header);
        const uint16_t *srcNotFound(BmpHeader &out_header);
    };

}