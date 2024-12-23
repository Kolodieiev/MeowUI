#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <IPAddress.h>
#include "UdpPacket.h"
#include <cstring>

namespace meow
{
    class ClientWrapper
    {
    public:
        ClientWrapper(IPAddress ip, uint16_t port) : _ip{ip}, _port{port} { _last_act_time = millis(); }

        IPAddress getIP() const { return _ip; }
        uint16_t getPort() const { return _port; }

        void confirm() { _is_confirmed = true; }
        bool isConfirmed() const { return _is_confirmed; }

        void prolong() { _last_act_time = millis(); }
        bool isConnected() const { return millis() - _last_act_time < 3000; }

        void setName(const char *name) { _name = name; }
        const char *getName() const { return _name.c_str(); }

        bool hasName(const char *name) const { return std::strcmp(_name.c_str(), name) == 0; }

        bool is(ClientWrapper *cl_wrap) const
        {
            if (!cl_wrap)
                return false;
            return _ip == cl_wrap->getIP();
        };

    protected:
        IPAddress _ip;
        uint16_t _port;

        String _name;
        bool _is_confirmed{false};

        unsigned long _last_act_time;
    };
} // namespace meow
