#pragma once
#include <stdint.h>

// -------------------------------- Додай ID екранів
namespace meow
{
    enum ScreenID : uint8_t
    {
        ID_SCREEN_SPLASH = 0,
        ID_SCREEN_HOME,
        ID_SCREEN_MENU,
        ID_SCREEN_TORCH,
        ID_SCREEN_GAMES,
        ID_SCREEN_MP3,
        ID_SCREEN_FILES,
        ID_SCREEN_FIRMWARE,
        ID_SCREEN_CALC,
        ID_SCREEN_READER,
        // Екрани налаштувань
        ID_SCREEN_PREF_SEL,
        ID_SCREEN_PREF_BRIGHT,
        ID_SCREEN_PREF_WATCH,
        ID_SCREEN_PREF_WALL,
        ID_SCREEN_PREF_FILE_SERVER,
        // Ігрові екрани
        ID_SCREEN_SNAKE,
        ID_SCREEN_SOKOBAN,
        ID_SCREEN_TEST_SERVER,
        //
    };
}
