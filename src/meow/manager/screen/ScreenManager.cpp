#pragma GCC optimize("O3")
#include "ScreenManager.h"
#include "../../ui/screen/IScreen.h"

#include "meowui_setup/screen_id.h"
#include "meowui_setup/ui_setup.h"

namespace meow
{
    ScreenManager MEOW;

    void ScreenManager::run()
    {
        _display.setRotation(DISPLAY_ROTATION);
        _display.init();

        IScreen *screen = new START_SCREEN(_display);

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
                SCREEN_CASES
                default:
                    log_e("Некоректний screen_id: %i", screen->getNextScreenID());
                    esp_restart();
                }
            }
        }
    }

}