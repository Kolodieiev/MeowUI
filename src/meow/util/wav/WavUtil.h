#pragma once
#include <Arduino.h>
#include "./WavHeader.h"
#include "./WavData.h"

namespace meow
{
    class WavUtil
    {
    public:
        WavData loadWav(const char *path_to_wav);

    private:
        const uint8_t HEADER_SIZE{44};

        bool validateHeader(const WavHeader &wav_header);
    };
}