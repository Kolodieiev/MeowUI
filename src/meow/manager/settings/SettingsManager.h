#pragma once
#include <Arduino.h>
#include "../files/FileManager.h"

namespace meow
{
    class SettingsManager : private FileManager
    {
    public:
        using FileManager::hasConnection;
        bool set(const char *pref_name, const char *value);
        String get(const char *pref_name);
        String getPrefFilePath(const char *pref_name);
    };
}