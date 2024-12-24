#pragma GCC optimize("O3")
#include "DataStream.h"

namespace meow
{
    DataStream::DataStream(size_t size)
    {
        _index = 0;
        _size = size;
        try
        {
            _buffer = new uint8_t[size];
        }
        catch (std::bad_alloc e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    DataStream::~DataStream()
    {
        delete _buffer;
    }

    size_t DataStream::read(char *out, size_t len)
    {
        if (!_buffer)
            return 0;

        if (len > _size)
            len = _size;

        memcpy(out, _buffer + _index, len);
        _index += len;
        return len;
    }

    size_t DataStream::write(const char *data, size_t len)
    {
        if (!_buffer)
            return 0;

        size_t s = space();
        if (len > s)
            len = s;

        memcpy(_buffer + _index, data, len);
        _index += len;
        return len;
    }

    size_t DataStream::space()
    {
        if (!_buffer)
            return 0;

        return _size - _index;
    }
}