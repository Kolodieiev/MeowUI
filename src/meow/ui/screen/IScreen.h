#pragma once
#include <Arduino.h>
#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../widget/layout/IWidgetContainer.h"

namespace meow
{

    class IScreen
    {
    public:
        // ----------------------------------------- Встанови ID екранів
        enum ScreenID : uint8_t
        {
            ID_SCREEN_HOME = 0,
            ID_SCREEN_GAME
        };

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
        inline uint8_t getNextScreenID() const { return _next_screen_ID; }
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
        void openScreenByID(uint8_t screen_ID);

    private:
        const uint8_t UI_UPDATE_DELAY = 25; // затримка між фреймами. 40FPS
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        uint8_t _next_screen_ID{0};
        //
#pragma endregion "don't touch this"
    };

}