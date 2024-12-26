#pragma GCC optimize("O3")
#include "DataStream.h"

namespace meow
{
    DataStream::DataStream(size_t size)
    {
        resize(size);
    }

    DataStream::~DataStream()
    {
        delete[] _buffer;
    }

    void DataStream::resize(size_t size)
    {
        _index = 0;
        _size = size;
        try
        {
            delete[] _buffer;
            _buffer = new uint8_t[size];
        }
        catch (std::bad_alloc e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    size_t DataStream::extractBytes(void *out, size_t start_pos, size_t len) const
    {
        ++start_pos;
        if (!_buffer || start_pos >= _size)
            return 0;

        size_t available = _size - start_pos;

        if (len > available)
            len = available;

        memcpy(out, _buffer + start_pos, len);

        return len;
    }

    size_t DataStream::read(void *out, size_t len)
    {
        if (!_buffer)
            return 0;

        size_t s = space();
        if (len > s)
            len = s;

        memcpy(out, _buffer + _index, len);
        _index += len;
        return len;
    }

    size_t DataStream::drop(size_t len)
    {
        if (!_buffer)
            return 0;

        size_t s = space();
        if (len > s)
            len = s;

        _index += len;

        return len;
    }

    size_t DataStream::write(void *data, size_t len)
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

    size_t DataStream::write(uint8_t data)
    {
        return write(&data, sizeof(data));
    }

    size_t DataStream::write(uint16_t data)
    {
        return write(&data, sizeof(data));
    }

    size_t DataStream::write(uint32_t data)
    {
        return write(&data, sizeof(data));
    }

    size_t DataStream::space()
    {
        if (!_buffer)
            return 0;

        return _size - _index;
    }
}