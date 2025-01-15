#pragma GCC optimize("O3")
#include "ContextManager.h"
#include "../../ui/context/IContext.h"

#include "meowui_setup/context_id.h"
#include "meowui_setup/ui_setup.h"

namespace meow
{
    ContextManager MEOW;

    void ContextManager::run()
    {
        _display.init();

        IContext *context = new START_CONTEXT(_display);

        // Основний цикл GUI.
        while (1)
        {
            if (!context->isReleased())
                context->_tick();
            else
            {
                delete context;
                switch (context->getNextContextID())
                {
                SCREEN_CASES
                default:
                    log_e("Некоректний context_id: %i", context->getNextContextID());
                    esp_restart();
                }
            }
        }
    }

}