#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/screen_id.h"

#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../widget/IWidgetContainer.h"

namespace meow
{
#define DWIDTH _display.width()
#define DHEIGHT _display.height()

    class IScreen
    {
    public:
#pragma region "don't touch this"
        IScreen(GraphicsDriver &display);
        virtual ~IScreen() = 0;
        IScreen(const IScreen &rhs) = delete;
        IScreen &operator=(const IScreen &rhs) = delete;
        //
        void tick();
        //
        inline ScreenID getNextScreenID() const { return _next_screen_ID; }
        inline bool isReleased() const { return _is_released; }

    protected:
        SemaphoreHandle_t _layout_mutex;
        //
        static Input _input;
        //
        GraphicsDriver &_display;
        //
        bool _screen_enabled{true}; // Прапор, який дозволяє вимкнути перерисовку екрану

        // Робота, повязана з відрисовкою GUI.
        virtual void update() = 0;

        // Код, який повинен виконуватися без затримок. Наприклад, відтворення аудіофайлів.
        virtual void loop() = 0;
        //
        void setLayout(IWidgetContainer *layout);
        inline IWidgetContainer *getLayout() const { return _layout; }
        void openScreenByID(ScreenID screen_ID);

        // Повертає х-координату, на якій віджет буде встановлено по центру відносно екрану.
        uint16_t getCenterX(IWidget *widget) const { return widget ? (DWIDTH - widget->getWidth()) / 2 : 0; }

    private:
        const uint8_t UI_UPDATE_DELAY = 25; // затримка між фреймами. 40FPS
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        ScreenID _next_screen_ID;
        //
#pragma endregion "don't touch this"
    };

}