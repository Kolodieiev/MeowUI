#pragma once
#include <Arduino.h>

namespace meow
{
    const uint8_t DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    struct DS3231DateTime
    {
        unsigned int second : 6;
        unsigned int minute : 6;
        unsigned int hour : 6;
        unsigned int day_of_month : 5;
        unsigned int month : 4;
        unsigned int year : 12;

        bool operator==(const DS3231DateTime &date_time) const
        {
            if (this == &date_time)
                return true;

            return this->minute == date_time.minute &&
                   this->hour == date_time.hour &&
                   this->day_of_month == date_time.day_of_month &&
                   this->month == date_time.month &&
                   this->year == date_time.year;
        }

        bool operator!=(const DS3231DateTime &date_time) const
        {
            return !(*this == date_time);
        }

        uint8_t dayOfWeek() const
        {
            uint16_t days = daysPassedSince2000(year - 2000, month, day_of_month);

            uint8_t day = (days + 6) % 7;

            if (day == 0)
                day = 6;
            else
                --day;

            return day;
        }

    private:
        uint16_t daysPassedSince2000(uint16_t year, uint8_t month, uint8_t day_of_month) const
        {
            uint16_t days = day_of_month;

            for (uint8_t index_month = 1; index_month < month; ++index_month)
                days += *(DAYS_IN_MONTH + index_month - 1);

            if (month > 2 && year % 4 == 0)
                ++days;

            return days + 365 * year + (year + 3) / 4 - 1;
        }
    };
}