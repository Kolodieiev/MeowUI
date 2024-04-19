#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"

using namespace meow;

class MenuScreen : public IScreen
{
public:
    MenuScreen(GraphicsDriver &display);
    virtual ~MenuScreen() {}

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

    enum Item_ID : uint8_t
    {
        ID_GAMES = 1,
        ID_MP3,
        ID_FILES,
        ID_PREFERENCES,
        ID_FIRMWARE,
        ID_EXMPL2,
        ID_EXMPL3,
    };

    FixedMenu *_menu;
    ScrollBar *_scrollbar;

    void up();
    void down();
    void ok();
};
