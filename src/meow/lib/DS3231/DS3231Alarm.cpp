#include "DS3231Alarm.h"
#include "DS3231Util.h"

namespace meow
{
    bool DS3231Alarm::isEnabled()
    {
        uint8_t ctrlReg = _i2c.readRegister(DS3231_ADDR, DS3231_REG_CTRL);
        return ctrlReg &= _BV(DS3231_AIFMASK);
    }

    bool DS3231Alarm::isAlarmed()
    {
        uint8_t statusReg = _i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS);
        return statusReg &= _BV(DS3231_CNTRL_BIT_A2IE);
    }

    void DS3231Alarm::setAlarmData(const DS3231AlarmTime &alarmData)
    {
        uint8_t buffer[3];

        uint8_t flags{0x08}; // hour minutes match

        buffer[0] = REG_ALARMTWO;
        buffer[1] = uint8ToBcd(alarmData.minute) | ((flags & 0x01) << 7);
        buffer[2] = uint8ToBcd(alarmData.hour) | ((flags & 0x02) << 6); // 24 hour mode only

        _i2c.write(DS3231_ADDR, buffer, 3);
    }

    void DS3231Alarm::enable(bool enableWithoutExternalPower)
    {
        // 6 bit change
        _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, 0b00000110);
    }

    void DS3231Alarm::disable()
    {
        _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, 0b00000000);
    }

    DS3231AlarmTime DS3231Alarm::getAlarmTime()
    {
        uint8_t buffer[REG_ALARMTWO_SIZE];
        buffer[0] = REG_ALARMTWO;

        if (!_i2c.write(DS3231_ADDR, buffer, 1))
            return DS3231AlarmTime{0, 0};

        if (!_i2c.read(DS3231_ADDR, buffer, REG_ALARMTWO_SIZE))
            return DS3231AlarmTime{0, 0};

        uint8_t minute = bcdToUint8(buffer[0] & 0x7F);
        uint8_t hour = bcdToBin24Hour(buffer[1] & 0x7f);

        return DS3231AlarmTime{hour, minute};
    }

    void DS3231Alarm::procAlarm()
    {
        uint8_t sreg = _i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS);
        sreg &= ~DS3231_AIFMASK; // clear the flags
        _i2c.writeRegister(DS3231_ADDR, DS3231_REG_STATUS, sreg);
    }
}