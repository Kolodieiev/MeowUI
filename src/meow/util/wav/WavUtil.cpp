#include "WavUtil.h"
#include <SD.h>

namespace meow
{
    WavData WavUtil::loadWav(const char *path_to_wav)
    {
        WavData wav_data;

        File wav_file = SD.open(path_to_wav, "r");

        if (!wav_file)
        {
            log_e("Помилка читання файлу: %s", path_to_wav);
            return wav_data;
        }

        if (wav_file.isDirectory())
        {
            wav_file.close();
            log_e("Помилка. Файл не може бути каталогом: %s", path_to_wav);
            return wav_data;
        }

        WavHeader header;

        wav_file.read((uint8_t *)&header, HEADER_SIZE);

        if (!validateHeader(header))
        {
            wav_file.close();
            log_e("Помилка валідації файлу: %s", path_to_wav);
            return wav_data;
        }

        if (!psramFound() || !psramInit())
        {
            wav_file.close();
            log_e("Помилка ініціалізації PSRAM");
            return wav_data;
        }

        uint8_t *data = (uint8_t *)ps_malloc(header.data_size);

        if (!data)
        {
            log_e("Помилка виділення пам'яті");
            wav_file.close();
            return wav_data;
        }

        size_t bytes_readed{0};
        wav_file.seek(HEADER_SIZE);
        bytes_readed = wav_file.read(data, header.data_size);

        if (bytes_readed != header.data_size)
        {
            wav_file.close();
            free(data);
            log_e("Помилка читання звуковго файлу");
            return wav_data;
        }

        wav_file.close();

        wav_data.size = header.data_size;
        wav_data.data_ptr = data;

        return wav_data;
    }

    bool WavUtil::validateHeader(const WavHeader &wav_header)
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