#pragma once
#include <Arduino.h>
#include "../ui/widget/menu/FixedMenu.h"
#include "../ui/widget/layout/EmptyLayout.h"

namespace meow
{

    class IGameMenu
    {
    public:
        IGameMenu(GraphicsDriver &display) : _display{display}
        {
            _menu = new FixedMenu(1, _display);
            _menu_back = new EmptyLayout(1, _display);
            _menu_back->addWidget(_menu);
        }

        virtual ~IGameMenu()
        {
            delete _menu_back;
        }

        virtual void onDraw() = 0;
        IGameMenu(const IGameMenu &rhs) = delete;
        IGameMenu &operator=(const IGameMenu &rhs) = delete;

        virtual void focusUp() = 0;
        virtual void focusDown() = 0;

        uint16_t getCurrentItemID() { return _menu->getCurrentItemID(); }

    protected:
        GraphicsDriver &_display;
        FixedMenu *_menu;
        EmptyLayout *_menu_back;
    };

}