#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "../AsyncTCP.h"

namespace meow
{
    class AsyncClientWrapper
    {
    public:
        AsyncClientWrapper(AsyncClient *client) : _client{client} {}
        ~AsyncClientWrapper()
        {
            if (!_is_freed)
            {
                _is_freed = true;
                _client->close();
                delete _client;
            }
        }
        void setName(const char *name) { _name = name; }
        const char *getName() const { return _name.c_str(); }
        bool is(AsyncClient *client) { return _client->getRemoteAddress() == client->getRemoteAddress(); }
        AsyncClient *getClient() { return _client; }
        uint32_t getAddress() const { return _client->getRemoteAddress(); }

        bool canSend() const { return _can_send; }
        void setSendTS()
        {
            _can_send = false;
            _packet_ts = millis();
        }
        void sendComplete() { _can_send = true; }
        bool isDisconnected() { return millis() - _packet_ts > SEND_ATTEMPTS_TIME; }

        void confirm() { _is_confirmed = true; }
        void unconfirm() { _is_confirmed = false; }
        bool isConfirmed() const { return _is_confirmed; }
        void markFreed() { _is_freed = true; }

    private:
        const uint16_t SEND_ATTEMPTS_TIME = 3000;
        unsigned long _packet_ts{0};
        AsyncClient *_client;
        String _name;
        bool _can_send{true};
        bool _is_confirmed{false};
        bool _is_freed{false};
    };
} // namespace meow
