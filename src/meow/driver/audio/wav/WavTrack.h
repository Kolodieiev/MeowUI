#pragma once
#include <Arduino.h>
#include "WavHeader.h"

namespace meow
{

    class WavTrack
    {
    public:
        WavTrack(const char *path);

        bool play();
        int16_t getNextSample();

        inline void setOnRepeat(bool repeate) { _on_repeate = repeate; }
        inline bool isPlaying() const { return _is_playing; };

        void setVolume(uint8_t volume);
        inline uint8_t getVolume() const { return _volume * MAX_VOLUME; }

        // Встановити коефіцієнт фільтрації шуму. По замовченню встановлено 1. Без фільтрації.
        void setFiltrationLvl(uint16_t lvl) { _filtration_lvl = lvl; }
        void freeData();

        WavTrack *clone();

    private:
        WavTrack() {}

        const uint8_t MAX_VOLUME{100};
        const float DEF_VOLUME{1.0f};

        float _volume{DEF_VOLUME};

        const uint8_t HEADER_SIZE{44};
        //
        uint8_t *_data_buf{nullptr};
        uint32_t _data_size;
        uint32_t _current_sample{0};
        //
        uint16_t _filtration_lvl{1};
        //
        bool _is_playing{false};
        bool _on_repeate{false};
        bool _has_init_err{false};
        //
        bool validateHeader(const WavHeader &wav_header);
    };

}