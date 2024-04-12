#include "GameClient.h"

namespace meow
{
    GameClient::GameClient()
    {
        _header.id = createID();
    }

    unsigned long GameClient::createID()
    {
        uint32_t id = micros();
        id <<= 16;
        id |= millis();
        return id;
    }
}