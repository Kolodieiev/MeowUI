#include "Preferences.h"

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
        String path = ROOT;

        if (!SD.exists(path))
            if (!SD.mkdir(path))
            {
                log_e("Помилка створення ROOT каталогу");
                return nullptr;
            }

        path += PREF_DIR;

        if (!SD.exists(path))
            if (!SD.mkdir(path))
            {
                log_e("Помилка створення PREF_DIR каталогу");
                return nullptr;
            }

        path += "/";
        path += file_name;

        File file;

        if (mode == "w")
            file = SD.open(path, mode, true);
        else
            file = SD.open(path, mode);

        if (!file)
        {
            log_e("Помилка читання файлу налаштувань: %s", path.c_str());
            return nullptr;
        }

        if (file.isDirectory())
        {
            log_e("Помилка. Файл налаштувань не може бути каталогом: %s", path.c_str());
            file.close();
            return nullptr;
        }

        return new File(file);
    }

}
