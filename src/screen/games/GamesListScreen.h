#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"

using namespace meow;

class GamesListScreen : public IScreen
{
public:
    GamesListScreen(GraphicsDriver &display);
    virtual ~GamesListScreen(){}

protected:
    virtual void loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_MENU = 1,
        ID_SCROLLBAR,
        ID_NAVBAR,
    };

    FixedMenu *_menu;
    ScrollBar *_scrollbar;

    void up();
    void down();
    void ok();
};
