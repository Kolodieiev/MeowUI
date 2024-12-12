#include "WiFiScanner.h"

bool meow::WiFiScanner::startScan()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    int16_t result_code = WiFi.scanNetworks(true);
    if (result_code == WIFI_SCAN_FAILED)
    {
        log_e("Помилка запуску сканера Wi-Fi");
        return false;
    }

    BaseType_t task_result = xTaskCreatePinnedToCore(waitFinishTask, "waitFinishTask", (1024 / 2) * 10, this, 10, NULL, 0);

    if (task_result == pdPASS)
    {
        log_i("waitFinishTask is working now");
        return true;
    }
    else
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        log_e("waitFinishTask was not running");
        return false;
    }
}

std::vector<String> meow::WiFiScanner::getScanResult() const
{
    std::vector<String> result_vec;

    int16_t scan_result = WiFi.scanComplete();

    for (uint16_t i = 0; i < scan_result; ++i)
        result_vec.emplace_back(WiFi.SSID(i));

    WiFi.scanDelete();

    return result_vec;
}

void meow::WiFiScanner::setOnDoneHandler(WiFiScanDoneHandler handler, void *args)
{
    _onDoneHandler = handler;
    _onDoneHandlerArgs = args;
}

void meow::WiFiScanner::callOnDoneHandler()
{
    if (_onDoneHandler)
        _onDoneHandler(_onDoneHandlerArgs);
    else
        WiFi.scanDelete();
}

void meow::WiFiScanner::waitFinishTask(void *params)
{
    WiFiScanner *this_ptr = static_cast<WiFiScanner *>(params);

    while (WiFi.scanComplete() < 0)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    this_ptr->callOnDoneHandler();

    vTaskDelete(NULL);
}
