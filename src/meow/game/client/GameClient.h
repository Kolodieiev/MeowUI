#pragma once
#include <Arduino.h>

namespace meow
{

    struct ClientHeader
    {
        unsigned long id{0};
        const char *name;
    };

    class GameClient
    {
    public:
        GameClient();
        ~GameClient() {}

        void setName(const char *name) { _header.name = name; }

    protected:
    private:
        ClientHeader _header;
        unsigned long createID();
    };

}