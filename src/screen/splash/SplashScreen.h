#pragma once
#include <Arduino.h>

#include "meow/ui/screen/IScreen.h"

using namespace meow;

class SplashScreen : public IScreen
{
public:
    SplashScreen(GraphicsDriver &display);
    virtual ~SplashScreen(){};

protected:
    virtual void loop() override;
    virtual void update() override;
    //
private:
    // const uint16_t SHOWING_INIT_TIME{2000};
    // const uint16_t SHOWING_LOGO_TIME{3000};

    const uint16_t SHOWING_INIT_TIME{1000};
    const uint16_t SHOWING_LOGO_TIME{1000};

    unsigned long _start_time;

    bool _logo_showed{false};

    void addLabel(uint16_t x_pos, uint16_t y_pos, const char *res_str, uint16_t color);

    uint8_t _widget_id{1};
};