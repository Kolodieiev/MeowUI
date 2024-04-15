#include "Preferences.h"
#include "../driver/spi/SpiHelper.h"

namespace meow
{
    const char ROOT[] = "/.data";
    const char PREF_DIR[] = "/preferences";

    bool Preferences::set(const char *pref_name, const char *value)
    {
        File *file = loadPrefFile(pref_name, "w");

        if (file == nullptr)
            return false;

        file->print(value);
        file->close();

        delete file;
        return true;
    }

    String Preferences::get(const char *pref_name)
    {
        File *file = loadPrefFile(pref_name, "r");

        if (file == nullptr)
            return String();

        String temp = file->readString();

        file->close();
        delete file;

        return temp;
    }

    File *Preferences::loadPrefFile(const char *file_name, const char *mode)
    {
        xSemaphoreTake(SpiHelper::_mutex, portMAX_DELAY);

        if (!SD.exists(ROOT))
            if (!SD.mkdir(ROOT))
            {
                log_e("Помилка створення ROOT каталогу");
                return nullptr;
            }

        if (!SD.exists(PREF_DIR))
            if (!SD.mkdir(PREF_DIR))
            {
                log_e("Помилка створення PREF_DIR каталогу");
                return nullptr;
            }

        xSemaphoreGive(SpiHelper::_mutex);

        String path = ROOT;
        path += "/";
        path += PREF_DIR;
        path += "/";
        path += file_name;

        File file;

        if (mode == "w")
            file = SD.open(path, mode, true);
        else
            file = SD.open(path, mode);

        if (!file)
        {
            log_e("Помилка читання файлу налаштувань: %s", path);
            return nullptr;
        }

        if (file.isDirectory())
        {
            log_e("Помилка. Файл налаштувань не може бути каталогом: %s", path);
            file.close();
            return nullptr;
        }

        return new File(file);
    }

}
