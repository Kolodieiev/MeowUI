#pragma once
#include <Arduino.h>

namespace meow
{
    class I2C
    {
    public:
        I2C();
        ~I2C();
        bool begin();
        void end();

        inline bool hasError() const { return _has_error; }

        bool hasConnection(uint8_t addr) const;

        bool write(uint8_t addr, const uint8_t *data_ptr, uint8_t data_size);
        bool writeRegister(uint8_t addr, uint8_t reg, uint8_t value);

        uint8_t readRegister(uint8_t addr, uint8_t reg);
        bool read(uint8_t addr, uint8_t *data_out_ptr, uint8_t data_size);

        void beginTransmission(uint8_t addr);
        bool endTransmission();

        bool send(uint8_t value);
        bool send(const uint8_t *data_ptr, uint8_t data_size);

        bool waitAndRead(uint8_t addr, uint8_t *data_out_ptr, uint8_t data_size);

    private:
        const uint8_t SDA_PIN{1};
        const uint8_t SCL_PIN{2};

        bool _has_error{false};
        bool _is_inited{false};

        bool checkInit() const;
    };
}