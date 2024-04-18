#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"
#include "meow/ui/widget/text/Label.h"

using namespace meow;

class HomeScreen : public IScreen
{
public:
    HomeScreen(GraphicsDriver &display);
    virtual ~HomeScreen();

protected:
    virtual void loop() override;
    virtual void update() override;
    //
private:
    enum Widget_ID : uint8_t
    {
        ID_WALLPAPER = 1,
        ID_BAT_LVL,
        ID_TIME_LBL,
        ID_DATE_LBL,
        ID_DAY_LBL,
        ID_NAVBAR
    };

    Image *_bat_ico;
    const uint16_t *_wallpaper_ptr{nullptr};
    unsigned long _upd_timer{0};
    uint8_t _h_counter{0};
    uint8_t _m_counter{0};
};
