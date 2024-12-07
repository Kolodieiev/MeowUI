#include "SettingsManager.h"
#include <SD.h>
#include "../sd/SD_Manager.h"

namespace meow
{
    const char DATA_ROOT[] = "/.data";
    const char PREF_ROOT[] = "/.data/preferences";

    bool SettingsManager::set(const char *pref_name, const char *value)
    {
        String path = getPrefFilePath(pref_name);
        if (path.isEmpty())
            return false;

        return writeFile(path.c_str(), value);
    }

    String SettingsManager::get(const char *pref_name)
    {
        String path = getPrefFilePath(pref_name);
        if (path.isEmpty())
            return "";

        size_t file_size = getFileSize(path.c_str());

        if (file_size == 0)
            return "";

        char *buffer = (char *)malloc(file_size + 1);

        if (!buffer)
        {
            log_e("Bad memory alloc: %zu b", file_size);
            return "";
        }

        size_t bytes_read = readFile(buffer, path.c_str(), file_size);
        buffer[bytes_read] = '\0';

        String ret;
        if (bytes_read > 0)
            ret = buffer;

        free(buffer);
        return ret;
    }

    String SettingsManager::getPrefFilePath(const char *pref_name)
    {
        if (!dirExist(DATA_ROOT))
            if (!createDir(DATA_ROOT))
                return "";

        if (!dirExist(PREF_ROOT))
            if (!createDir(PREF_ROOT))
                return "";

        String path = PREF_ROOT;
        path += "/";
        path += pref_name;

        return path;
    }
}
