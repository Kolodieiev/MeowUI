#include "ScreenManager.h"
#include "./screen/IScreen.h"
#include "./ScreenID.h"

// ------------------------------------------------------ Підключи тут екрани

#include "screen/splash/SplashScreen.h"
#include "screen/home/HomeScreen.h"
#include "screen/menu/MenuScreen.h"
#include "screen/games/GamesListScreen.h"
// Ігрові екрани
#include "screen/games/sokoban/SokobanScreen.h"

// ------------------------------------------------------

namespace meow
{
    ScreenManager MEOW;

#define DISPLAY_ROTATION 3 // --------------------------- Стартова орієнтація екрану

    void ScreenManager::run()
    {
        _display.setRotation(DISPLAY_ROTATION);
        _display.init();

        // -------------------------------------------------- Встанови стартовий екран
        IScreen *screen = new SplashScreen(_display);
        screen->show();

        // Основний цикл GUI.
        while (1)
        {
            if (!screen->isReleased())
                screen->tick();
            else
            {
                delete screen;
                switch (screen->getNextScreenID())
                {
                // --------------------------------------- Відредагуй switch під свої екрани
                case ScreenID::ID_SCREEN_HOME:
                    screen = new HomeScreen(_display);
                    break;
                case ScreenID::ID_SCREEN_MENU:
                    screen = new MenuScreen(_display);
                    break;
                case ScreenID::ID_SCREEN_GAMES_LIST:
                    screen = new GamesListScreen(_display);
                    break;
                    // --------------------------------------- Ігрові екрани
                case ScreenID::ID_SCREEN_SOKOBAN:
                    screen = new SokobanScreen(_display);
                    break;
                default:
                    log_e("Некоректний screen_id: %i", screen->getNextScreenID());
                    esp_restart();
                }
                screen->show();
            }
        }
    }

}