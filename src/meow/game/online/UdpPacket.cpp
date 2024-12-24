#pragma GCC optimize("O3")
#include "UdpPacket.h"
#include <cstring>

meow::UdpPacket::UdpPacket(AsyncUDPPacket &packet) : DataStream(packet.length() < 2 ? 2 : packet.length() + 1)
{
    try
    {
        _buffer[0] = CMD_NONE;
        _buffer[_size - 1] = '\0';

        --_size;
        _index = 1;

        memcpy(_buffer, packet.data(), _size);

        _ip = packet.remoteIP();
        _port = packet.remotePort();
    }
    catch (std::exception e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

meow::UdpPacket::UdpPacket(size_t data_len) : DataStream(data_len + 2)
{
    try
    {
        _buffer[0] = CMD_NONE;
        _buffer[_size - 1] = '\0';

        --_size; // Hide \0
        _index = 1;
    }
    catch (std::bad_alloc e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

meow::UdpPacket::UdpPacket(void *raw_data, size_t len) : DataStream(len < 2 ? 2 : len + 1)
{
    try
    {
        _buffer[_size - 1] = '\0';

        --_size; // Hide \0
        _index = 1;

        memcpy(_buffer, (const uint8_t *)raw_data, _size);
    }
    catch (std::exception e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

void meow::UdpPacket::setCommand(Command cmd)
{
    _buffer[0] = cmd;
}

meow::UdpPacket::Command meow::UdpPacket::getCommand()
{
    return static_cast<Command>(_buffer[0]);
}

void meow::UdpPacket::setData(void *data)
{
    memcpy(&_buffer[1], (const uint8_t *)data, _size - 1);
}

const char *meow::UdpPacket::getData(uint16_t data_pos)
{
    if (data_pos >= _size)
    {
        log_e("Некоректний запит. Розмір: %zu Запит: %d ", _size - 1, data_pos);
        data_pos = 0;
    }

    return (const char *)&_buffer[1 + data_pos];
}

void meow::UdpPacket::printToLog()
{
    log_i("Command: %d", _buffer[0]);
    log_i("Data size: %zu", _size);
    log_i("Data:");
    for (size_t i = 1; i < _size; ++i)
        log_i("%c", _buffer[i]);
}

bool meow::UdpPacket::isDataEquals(const char *data, size_t start_pos, size_t data_len)
{
    ++start_pos;

    if (!data || start_pos >= _size)
        return false;

    if (data_len == 0)
        data_len = _size - start_pos;
    else if (start_pos + data_len > _size)
        return false;

    return std::memcmp(&_buffer[start_pos], data, data_len) == 0;
}

bool meow::UdpPacket::isDataEquals(const uint8_t *data, size_t start_pos, size_t data_len)
{
    return isDataEquals(reinterpret_cast<const char *>(data), start_pos, data_len);
}
