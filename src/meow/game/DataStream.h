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
        void resize(size_t size);
        size_t extractBytes(void *out, size_t start_pos = 0, size_t len = 1) const;
        size_t read(void *out, size_t len = 1);
        size_t drop(size_t len = 1);
        size_t write(void *data, size_t len = 1);
        size_t write(uint8_t data);
        size_t write(uint16_t data);
        size_t write(uint32_t data);
        size_t space();
        uint8_t *raw() { return _buffer; }
        size_t length() const { return _size; }
        size_t index() const { return _index - 1; }
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