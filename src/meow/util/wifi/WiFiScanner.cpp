#include "WiFiScanner.h"

namespace meow
{
    WiFiScanner *WiFiScanner::_instance;

    WiFiScanner::WiFiScanner()
    {
        _instance = this;
    }

    bool WiFiScanner::startScan()
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        delay(100);

        WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);

        int16_t result_code = WiFi.scanNetworks(true);
        if (result_code == WIFI_SCAN_FAILED)
        {
            log_e("Помилка запуску сканера Wi-Fi");
            return false;
        }

        return true;
    }

    void WiFiScanner::getScanResult(std::vector<String> &out_vector) const
    {
        std::vector<String> result_vec;
        int16_t scan_result = WiFi.scanComplete();

        for (uint16_t i = 0; i < scan_result; ++i)
            result_vec.emplace_back(WiFi.SSID(i));

        WiFi.scanDelete();

        return result_vec;
    }

    void WiFiScanner::setOnDoneHandler(WiFiScanDoneHandler handler, void *args)
    {
        _onDoneHandler = handler;
        _onDoneHandlerArgs = args;
    }

    void WiFiScanner::callOnDoneHandler()
    {
        if (_onDoneHandler)
            _onDoneHandler(_onDoneHandlerArgs);
        else
            WiFi.scanDelete();
    }

    void WiFiScanner::onEvent(WiFiEvent_t event)
    {
        WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
        _instance->callOnDoneHandler();
    }
}