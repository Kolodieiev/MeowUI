#include "WavTrack.h"
#include "math.h"

namespace meow
{

    WavTrack::WavTrack(const char *path, fs::FS &fs)
    {
        File wav_file = SD.open(path, "r");

        if (!wav_file)
        {
            _has_init_err = true;
            log_e("Помилка читання файлу: %s", path);
            return;
        }

        if (wav_file.isDirectory())
        {
            wav_file.close();
            _has_init_err = true;
            log_e("Помилка. Файл не може бути каталогом: %s", path);
            return;
        }

        WavHeader header;

        wav_file.read((uint8_t *)&header, HEADER_SIZE);

        if (!validateHeader(header))
        {
            wav_file.close();
            _has_init_err = true;
            log_e("Помилка валідації файлу: %s", path);
            return;
        }

        if (!psramFound())
        {
            wav_file.close();
            _has_init_err = true;
            log_e("Помилка. PSRAM не знайдено");
            return;
        }

        if (!psramInit())
        {
            wav_file.close();
            _has_init_err = true;
            log_e("Помилка ініціалізації PSRAM");
            return;
        }

        _data_size = header.data_size;
        _data_buf = (uint8_t *)ps_malloc(_data_size);

        if (_data_buf != NULL && _data_buf != nullptr)
        {
            size_t bytes_readed{0};
            wav_file.seek(HEADER_SIZE);
            bytes_readed += wav_file.read(_data_buf, _data_size);

            // log_d("DATA_SIZE: %lu", _data_size);
            // log_d("bytes_readed: %lu", bytes_readed);
            // log_d("Free PSRAM: %lu", ESP.getFreePsram());

            if (bytes_readed != _data_size)
            {
                _has_init_err = true;
                log_e("Помилка читання звуковго файлу");
                free(_data_buf);
                _data_buf = nullptr;
            }
        }
        else
        {
            _has_init_err = true;
            log_e("Помилка виділення памяті PSRAM");
        }

        wav_file.close();
    }

    bool WavTrack::play()
    {
        if (_has_init_err)
            return false;

        _is_playing = true;
        _current_sample = 0;

        return true;
    }

    int16_t WavTrack::getNextSample()
    {
        if (!_is_playing)
            return 0;

        int16_t ret = *((int16_t *)(_data_buf + _current_sample));
        ret = static_cast<int16_t>(ret * _volume);

        if (std::abs(ret) > _volume * _filtration_lvl)
        {
            int8_t high = ret;
            ret >>= 8;
            int8_t low = ret;

            ret = high;
            ret <<= 8;
            ret |= low;
        }
        else
        {
            ret = 0;
        }

        _current_sample += 2;

        if (_current_sample >= _data_size)
        {
            if (!_on_repeate)
                _is_playing = false;

            _current_sample = 0;
        }

        return ret;
    }

    void WavTrack::setVolume(uint8_t volume)
    {
        if (volume < MAX_VOLUME)
            _volume = (float)volume / 100;
        else
            _volume = DEF_VOLUME;
    }

    void WavTrack::freeData()
    {
        if (_data_buf == nullptr || _data_buf == NULL)
            return;

        free(_data_buf);
    }

    WavTrack *WavTrack::clone()
    {
        WavTrack *ret = new WavTrack(*this);
        if (!ret)
        {
            log_e("Помилка клонування звукової доріжки");
            esp_restart();
        }

        return ret;
    }

    bool WavTrack::validateHeader(const WavHeader &wav_header)
    {
        if (memcmp(wav_header.riff_section_ID, "RIFF", 4) != 0)
        {
            log_e("Не RIFF формат");
            return false;
        }
        if (memcmp(wav_header.riff_format, "WAVE", 4) != 0)
        {
            log_e("Не Wav файл");
            return false;
        }
        if (memcmp(wav_header.format_section_ID, "fmt", 3) != 0)
        {
            log_e("Відсутній format_section_ID");
            return false;
        }
        if (memcmp(wav_header.data_section_ID, "data", 4) != 0)
        {
            log_e("Відсутній data_section_ID");
            return false;
        }
        if (wav_header.format_ID != 1)
        {
            log_e("format_ID повинен == 1");
            return false;
        }
        if (wav_header.format_size != 16)
        {
            log_e("format_size повинен бути == 16");
            return false;
        }
        if ((wav_header.num_channels != 1))
        {
            log_e("Підтримується тільки моно формат");
            return false;
        }
        if (wav_header.sample_rate != 16000)
        {
            log_e("Частота дескритизації повинна == 16000");
            return false;
        }
        if (wav_header.bits_per_sample != 16)
        {
            log_e("Підтримуєтсья тільки 16 біт на семпл");
            return false;
        }
        return true;
    }

}