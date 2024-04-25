#pragma once
#include <Arduino.h>

// ----------------------------------------- Встанови ID екранів
enum ScreenID : uint8_t
{
    ID_SCREEN_SPLASH = 0,
    ID_SCREEN_HOME,
    ID_SCREEN_MENU,
    ID_SCREEN_GAMES_LIST,
    ID_SCREEN_MP3,
    ID_SCREEN_FILES,
    ID_SCREEN_PREF,
    ID_SCREEN_FIRMWARE,
    // Ігрові екрани
    ID_SCREEN_SNAKE,
    ID_SCREEN_SOKOBAN
};