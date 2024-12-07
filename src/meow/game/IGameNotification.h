#pragma once
#include <Arduino.h>
#include "../driver/graphics/GraphicsDriver.h"
#include "../ui/widget/notification/Notification.h"

namespace meow
{

    class IGameNotification
    {
    public:
        IGameNotification(GraphicsDriver &display) : _display{display}
        {
            try
            {
                _notification = new Notification(1, _display);
            }
            catch (const std::bad_alloc &e)
            {
                log_e("%s", e.what());
                esp_restart();
            }
        }

        virtual ~IGameNotification()
        {
            delete _notification;
        }

        virtual void onDraw() = 0;
        IGameNotification(const IGameNotification &rhs) = delete;
        IGameNotification &operator=(const IGameNotification &rhs) = delete;

        inline void show() { _notification->show(); }
        inline void hide() { _notification->hide(); }
        inline bool isHidden() const { return _notification->isHidden(); }

    protected:
        GraphicsDriver &_display;
        Notification *_notification{nullptr};
    };

}