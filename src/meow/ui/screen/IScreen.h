#pragma once
#include <Arduino.h>
#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../../driver/spi/SpiHelper.h"
#include "../widget/layout/IWidgetContainer.h"
#include "../ScreenID.h"

namespace meow
{
    class IScreen
    {
    public:
#pragma region "don't touch this"
        IScreen(GraphicsDriver &display);
        virtual ~IScreen() = 0;
        IScreen(const IScreen &rhs) = delete;
        IScreen &operator=(const IScreen &rhs) = delete;
        //
        // Відобразити екран
        void show();
        void tick();
        //
        inline ScreenID getNextScreenID() const { return _next_screen_ID; }
        inline bool isReleased() const { return _is_released; }

    protected:
        //
        static Input _input;
        //
        GraphicsDriver &_display;
        //

        // Робота, повязана з відрисовкою GUI.
        virtual void update() = 0;

        // Код, який повинен виконуватися без затримок. Наприклад, відтворення аудіофайлів.
        virtual void loop() = 0;
        //
        void setLayout(IWidgetContainer *layout);
        inline IWidgetContainer *getLayout() const { return _layout; }
        void openScreenByID(ScreenID screen_ID);

    private:
        const uint8_t UI_UPDATE_DELAY = 25; // затримка між фреймами. 40FPS
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        ScreenID _next_screen_ID{ScreenID::ID_SCREEN_HOME};
        //
#pragma endregion "don't touch this"
    };

}