#include "BmpUtil.h"
#include <SD.h>

namespace meow
{
    BmpData BmpUtil::loadBmp(const char *path_to_bmp)
    {
        File bmp_file = SD.open(path_to_bmp, "r");
        if (!bmp_file)
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            return srcNotFound();
        }

        if (bmp_file.isDirectory())
        {
            bmp_file.close();
            log_e("Помилка. Файл не може бути каталогом: %s", path_to_bmp);
            return srcNotFound();
        }

        BmpHeader bmp_header;
        bmp_file.read((uint8_t *)&bmp_header, 54);
        if (!validateHeader(bmp_header))
        {
            bmp_file.close();
            log_e("Помилка валідації файлу: %s", path_to_bmp);
            return srcNotFound();
        }

        if (!psramFound() || !psramInit())
        {
            bmp_file.close();
            log_e("Помилка ініціалізації PSRAM");
            return srcNotFound();
        }
        //
        bool is_flipped = bmp_header.height > 0;
        int32_t height = std::abs(bmp_header.height);
        bmp_header.height = height;
        int32_t width = bmp_header.width;
        //
        uint32_t data_size = width * height * 2;
        //
        uint8_t *data = (uint8_t *)ps_malloc(data_size);
        if (!data)
        {
            bmp_file.close();
            log_e("Помилка виділення %lu байт PSRAM памяті", data_size);
            return srcNotFound();
        }

        bmp_file.seek(bmp_header.data_offset);

        size_t bytes_read = bmp_file.read(data, data_size);

        if (bytes_read != data_size)
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            free(data);
            bmp_file.close();
            return srcNotFound();
        }

        bmp_file.close();

        uint16_t *data_temp = (uint16_t *)data;

        if (is_flipped)
        {
            uint16_t temp;
            uint32_t d_size = width * height;

            for (uint32_t i = 0; i < d_size * 0.5; ++i)
            {
                temp = data_temp[i];
                data_temp[i] = data_temp[d_size - i - 1];
                data_temp[d_size - i - 1] = temp;
            }
        }

        for (uint32_t y = 0; y < height; ++y)
        {
            uint16_t *row_start = data_temp + y * width;

            for (uint32_t x = 0; x < width / 2; ++x)
            {
                uint16_t temp = row_start[x];
                row_start[x] = row_start[width - x - 1];
                row_start[width - x - 1] = temp;
            }
        }

        BmpData bmp_data;

        bmp_data.width = bmp_header.width;
        bmp_data.height = bmp_header.height;
        bmp_data.is_loaded = true;
        bmp_data.data_ptr = (uint16_t *)data;

        return bmp_data;
    }

    bool BmpUtil::validateHeader(const BmpHeader &bmp_header)
    {
        if ((bmp_header.file_type != 0x4D42))
        {
            log_e("Не bmp");
            return false;
        }

        if ((bmp_header.bit_pp != 16))
        {
            log_e("Зображення повинне мати 16bpp");
            return false;
        }

        if ((bmp_header.width == 0 || bmp_header.height == 0))
        {
            log_e("Зображення містить некоректний заголовок");
            return false;
        }

        return true;
    }

    BmpData BmpUtil::srcNotFound()
    {
        BmpData bmp_data;
        bmp_data.width = 32;
        bmp_data.height = 32;
        bmp_data.data_ptr = SRC_NOT_FOUND;
        return bmp_data;
    }

    bool BmpUtil::saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp)
    {
        File bmp_file = SD.open(path_to_bmp, "w", true);
        if (!bmp_file)
        {
            log_e("Помилка відкриття файлу: %s", path_to_bmp);
            return false;
        }

        if (bmp_file.isDirectory())
        {
            bmp_file.close();
            log_e("Помилка. Файл не може бути каталогом: %s", path_to_bmp);
            return false;
        }

        header.image_size = header.width * header.height * 2;
        header.file_size = header.data_offset + header.image_size;
        uint32_t buf_size = header.width * header.height;
        header.height *= -1;

        uint8_t *data = (uint8_t *)ps_malloc(header.file_size);
        if (!data)
        {
            bmp_file.close();
            log_e("Помилка виділення %lu байт PSRAM памяті", header.file_size);
            return false;
        }

        uint8_t *header_ptr = (uint8_t *)&header;
        for (uint8_t i = 0; i < header.data_offset; ++i)
            data[i] = header_ptr[i];

        uint16_t *data_p16 = reinterpret_cast<uint16_t *>(data + header.data_offset);
        for (int i = 0; i < buf_size; ++i)
            data_p16[i] = __bswap16(buff[i]);

        size_t written_bytes = bmp_file.write(data, header.file_size);

        bmp_file.close();
        free(data);

        if (written_bytes != header.file_size)
        {
            log_e("Помилка запису файлу: %s", path_to_bmp);
            return false;
        }
        else
            return true;
    }
}