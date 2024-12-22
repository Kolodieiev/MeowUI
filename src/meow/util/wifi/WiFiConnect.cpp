#include "WiFiConnect.h"

meow::WiFiConnect *meow::WiFiConnect::_instance;

meow::WiFiConnect::WiFiConnect()
{
    _instance = this;
}

bool meow::WiFiConnect::tryConnectTo(String ssid, String pwd, int32_t wifi_chan, bool autoreconnect)
{
    WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.setAutoReconnect(autoreconnect);

    wl_status_t status = WiFi.begin(ssid, pwd, wifi_chan);

    if (status != WL_DISCONNECTED)
    {
        log_e("Помилка приєднання до: %s", ssid.c_str());
        WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
        return false;
    }

    return true;
}

void meow::WiFiConnect::setOnDoneHandler(WiFiConnectHandler handler, void *args)
{
    _onConnectHandler = handler;
    _onConnectHandlerArgs = args;
}

void meow::WiFiConnect::setWiFiPower(WiFiPowerLevel power_lvl)
{
    switch (power_lvl)
    {
    case WIFI_POWER_MIN:
        WiFi.setTxPower(WIFI_POWER_5dBm);
        break;
    case WIFI_POWER_MEDIUM:
        WiFi.setTxPower(WIFI_POWER_15dBm);
        break;
    case WIFI_POWER_MAX:
        WiFi.setTxPower(WIFI_POWER_19_5dBm);
        break;
    }
}

void meow::WiFiConnect::disconnect()
{
    _onConnectHandler = nullptr;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void meow::WiFiConnect::callOnDoneHandler()
{
    if (!_onConnectHandler)
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
    else
    {
        log_i("WiFi.status: %d", WiFi.status());
        _onConnectHandler(_onConnectHandlerArgs, WiFi.status());
    }
}

void meow::WiFiConnect::onEvent(WiFiEvent_t event)
{
    WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP)
    {
        long unsigned got_ip_time = millis();
        while (millis() - got_ip_time < 2000 || WiFi.status() != WL_CONNECTED)
            vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    _instance->callOnDoneHandler();
}
