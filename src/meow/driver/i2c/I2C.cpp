#pragma GCC optimize("O3")

#include "I2C.h"
#include <Wire.h>

namespace meow
{
    I2C::I2C()
    {
    }

    I2C::~I2C()
    {
        end();
    }

    bool I2C::begin()
    {
        if (!_is_inited)
        {
            _is_inited = Wire.begin(SDA_PIN, SCL_PIN);

            if (!_is_inited)
                log_e("Помилка ініціалізіції I2C");
        }

        return _is_inited;
    }

    void I2C::end()
    {
        Wire.end();
        Wire.flush();
    }

    bool I2C::hasConnection(uint8_t addr) const
    {
        if (!checkInit())
            return false;

        Wire.beginTransmission(addr);
        return !Wire.endTransmission();
    }

    bool I2C::writeRegister(uint8_t addr, uint8_t reg, uint8_t value)
    {
        if (!checkInit())
            return false;

        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(value);
        return !Wire.endTransmission();
    }

    bool I2C::write(uint8_t addr, const uint8_t *data_ptr, uint8_t data_size)
    {
        if (!checkInit())
            return false;

        Wire.beginTransmission(addr);
        Wire.write(data_ptr, data_size);
        return !Wire.endTransmission();
    }

    uint8_t I2C::readRegister(uint8_t addr, uint8_t reg)
    {
        _has_error = false;

        if (!checkInit())
        {
            _has_error = true;
            return 0;
        }

        Wire.beginTransmission(addr);
        Wire.write(reg);
        _has_error = Wire.endTransmission();

        if (_has_error)
            return 0;

        bool is_success_request = Wire.requestFrom(addr, (uint8_t)1) == 1;

        if (!is_success_request)
        {
            _has_error = true;
            return 0;
        }

        return Wire.read();
    }

    bool I2C::read(uint8_t addr, uint8_t *data_out_ptr, uint8_t data_size)
    {
        if (!checkInit())
            return false;

        bool is_success_request = Wire.requestFrom(addr, data_size) == data_size;

        if (!is_success_request)
            return false;

        for (uint8_t i = 0; i < data_size; ++i)
            data_out_ptr[i] = Wire.read();

        return true;
    }

    void I2C::beginTransmission(uint8_t addr)
    {
        if (!checkInit())
            return;

        Wire.beginTransmission(addr);
    }

    bool I2C::endTransmission()
    {
        return !Wire.endTransmission();
    }

    bool I2C::send(uint8_t value)
    {
        return Wire.write(value) == 1;
    }

    bool I2C::send(const uint8_t *data_ptr, uint8_t data_size)
    {
        return Wire.write(data_ptr, data_size) == data_size;
    }

    bool I2C::waitAndRead(uint8_t addr, uint8_t *data_out_ptr, uint8_t data_size)
    {
        if (!checkInit())
            return false;

        if (Wire.requestFrom(addr, data_size) == data_size)
            return false;

        unsigned long curTime = millis();
        const unsigned long AWAITING_TIME{200};

        while (!Wire.available() && (millis() - curTime) < AWAITING_TIME)
        {
        }

        if (Wire.available())
            for (uint8_t i = 0; i < data_size; ++i)
                data_out_ptr[i] = Wire.read();

        return true;
    }

    bool I2C::checkInit() const
    {
        if (!_is_inited)
        {
            log_e("I2C не ініціалізовано");
            return false;
        }

        return true;
    }
}