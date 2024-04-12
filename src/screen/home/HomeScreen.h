#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/ui/widget/text/Label.h"

using namespace meow;

class HomeScreen : public IScreen
{
public:
    HomeScreen(GraphicsDriver &display);
    virtual ~HomeScreen(){};

protected:
    virtual void loop() override;
    virtual void update() override;
    //
private:
    ScrollBar *_scrollbar;
    FixedMenu *_menu;

    void up();
    void down();
    void ok();
};
