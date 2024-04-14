#include "Meow.h"
#include "./screen/IScreen.h"
#include "./ScreenID.h"

// ------------------------------------------------------ Підключи тут екрани
#include "screen/home/HomeScreen.h"
// #include "screen/game/GameScreen.h"

namespace meow
{
    Meow MEOW;

#define DISPLAY_ROTATION 3 // --------------------------- Стартова орієнтація екрану

    void Meow::run()
    {
        _display.setRotation(DISPLAY_ROTATION);
        _display.init();

        // -------------------------------------------------- Встанови стартовий екран
        IScreen *screen = new HomeScreen(_display);
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
                // ---------------------------------------
                case ScreenID::ID_SCREEN_HOME:
                    screen = new HomeScreen(_display);
                    break;
                // case ScreenID::ID_SCREEN_GAME:
                //     screen = new GameScreen(_display);
                //     break;
                default:
                    log_e("Невідомий screen_id: %i", screen->getNextScreenID());
                    esp_restart();
                }
                screen->show();
            }
        }
    }

}