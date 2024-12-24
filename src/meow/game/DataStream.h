#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class DataStream
    {
    public:
        DataStream(size_t size);
        ~DataStream();
        size_t read(char *out, size_t len = 1);
        size_t write(const char *data, size_t len = 1);
        size_t space();
        uint8_t *raw() { return _buffer; }
        size_t length() const { return _size; }
        void flush() { _index = 0; }
        operator bool()
        {
            return _buffer != nullptr;
        }

    protected:
        uint8_t *_buffer{nullptr};
        size_t _index;
        size_t _size;
    };
}