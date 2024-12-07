#include "SD_Manager.h"

namespace meow
{
    SD_Manager SD_MNGR = SD_Manager::getInstance();
    bool SD_Manager::_is_inited = false;
}

void meow::SD_Manager::setup(uint8_t cs_pin, SPIClass *spi, uint32_t frequency, const char *mountpoint, uint8_t max_files)
{
    _cs_pin = cs_pin;
    _spi = spi;
    _frequency = frequency;
    _mountpoint = mountpoint;
    _max_files = max_files;
    _has_setup = true;
}

bool meow::SD_Manager::testConection() const
{
    if (!_is_mounted)
        return false;

    File test_file = SD.open("/");
    if (test_file)
    {
        test_file.close();
        return true;
    }

    return false;
}

bool meow::SD_Manager::mount()
{
    if (_is_mounted)
        return true;

    if (!_has_setup)
        setup();

    _is_mounted = SD.begin(_cs_pin, *_spi, _frequency, _mountpoint, _max_files);

    if (!_is_mounted)
        log_e("Помилка ініціалізації SD");

    return _is_mounted;
}

void meow::SD_Manager::unmount()
{
    SD.end();
    _is_mounted = false;
}

meow::SD_Manager meow::SD_Manager::getInstance()
{
    if (!_is_inited)
    {
        SD_MNGR = SD_Manager();
        _is_inited = true;
    }

    return SD_MNGR;
}
