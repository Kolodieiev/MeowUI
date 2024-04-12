#pragma once
#include <Arduino.h>
#include <SD.h>

namespace meow
{

    class Preferences
    {
    public:
        bool set(const char *pref_name, const char *value);
        String get(const char *pref_name);

    private:
        File *loadPrefFile(const char *file_name, const char *mode);
    };

}