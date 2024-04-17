#include "BmpUtil.h"
#include <SD.h>
#include "./src_not_found.h"

namespace meow
{

    const uint16_t *BmpUtil::loadBmp(const char *path_to_bmp, BmpHeader &out_header)
    {
        File bmp_file = SD.open(path_to_bmp, "r");
        if (!bmp_file)
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            return srcNotFound(out_header);
        }

        if (bmp_file.isDirectory())
        {
            bmp_file.close();
            log_e("Помилка. Файл не може бути каталогом: %s", path_to_bmp);
            return srcNotFound(out_header);
        }

        bmp_file.read((uint8_t *)&out_header, BMP_HEADER_SIZE + BMP_INFO_SIZE);

        if (!validateHeader(out_header))
        {
            bmp_file.close();
            log_e("Помилка валідації файлу: %s", path_to_bmp);
            return srcNotFound(out_header);
        }

        if (!psramFound() || !psramInit())
        {
            bmp_file.close();
            log_e("Помилка ініціалізації PSRAM");
            return srcNotFound(out_header);
        }
        //
        bool is_flipped = out_header.height > 0;
        int32_t height = std::abs(out_header.height);
        out_header.height = height;
        int32_t width = out_header.width;
        //
        uint32_t data_size = width * height * 2;
        //
        uint8_t *data = (uint8_t *)ps_malloc(data_size);
        if (!data)
        {
            bmp_file.close();
            log_e("Помилка виділення %lu байт PSRAM памяті", data_size);
            return srcNotFound(out_header);
        }

        bmp_file.seek(out_header.data_offset);

        size_t bytes_readed = bmp_file.read(data, data_size);

        if (bytes_readed != data_size)
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            free(data);
            bmp_file.close();
            return srcNotFound(out_header);
        }

        bmp_file.close();

        if (is_flipped)
        {
            uint16_t temp;
            uint16_t *data_temp = (uint16_t *)data;
            uint16_t d_size = width * height;

            for (int32_t i = 0; i < d_size * 0.5; ++i)
            {
                temp = data_temp[i];
                data_temp[i] = data_temp[d_size - i - 1];
                data_temp[d_size - i - 1] = temp;
            }
        }

        return (uint16_t *)data;
    }

    bool BmpUtil::validateHeader(const BmpHeader &bmp_header)
    {
        if ((bmp_header.file_type != 0x4D42))
        {
            log_e("Не bmp");
            return false;
        }

        if ((bmp_header.bit_count != 16))
        {
            log_e("Зображення повинне мати 16bpp");
            return false;
        }

        if ((bmp_header.width == 0 || bmp_header.height == 0))
        {
            log_e("Зображення не містить дані розміру");
            return false;
        }

        return true;
    }

    const uint16_t *BmpUtil::srcNotFound(BmpHeader &out_header)
    {
        out_header.file_type = 0;
        out_header.width = 32;
        out_header.height = 32;
        return SRC_NOT_FOUND;
    }

    bool BmpUtil::saveBmp(BmpHeader &header, const uint16_t *data, const char *path_to_bmp)
    {
        // TODO Реалізувати
        log_e("Не реалізовано");
        esp_restart();
        return false;
    }
}