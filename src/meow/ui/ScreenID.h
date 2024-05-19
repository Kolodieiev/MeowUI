#pragma once
#include <Arduino.h>

// ----------------------------------------- Встанови ID екранів
enum ScreenID : uint8_t
{
    ID_SCREEN_SPLASH = 0,
    ID_SCREEN_HOME,
    ID_SCREEN_MENU,
    ID_SCREEN_GAMES_LIST,
    // Ігрові екрани
    ID_SCREEN_SNAKE,
    ID_SCREEN_SOKOBAN
};