#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>

namespace meow
{
    class UdpPacket : public AsyncUDPMessage
    {
    public:
        enum Command : uint8_t
        {
            CMD_NONE = 0,
            CMD_HANDSHAKE,
            CMD_NAME,
            CMD_DATA,
            CMD_BUSY,
            CMD_PING,
        };

        UdpPacket(AsyncUDPPacket &packet);
        UdpPacket(size_t data_len);
        UdpPacket(void *raw_data, size_t len);

        void setCommand(Command cmd);
        Command getCommand();
        //
        void setData(void *data);
        const char *getData(uint16_t data_pos = 0);
        //
        size_t dataLen() { return _size - 1; }
        //
        void printToLog();
        //
        IPAddress getRemoteIP() const { return _ip; }
        uint16_t getRemotePort() const { return _port; }
        //
        bool isDataEquals(const char *data, size_t start_pos = 0, size_t data_len = 0);

    private:
        using AsyncUDPMessage::flush;
        using AsyncUDPMessage::space;
        using AsyncUDPMessage::write;

        IPAddress _ip;
        uint16_t _port{0};
    };
}