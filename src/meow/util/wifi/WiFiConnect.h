#pragma once
#include <Arduino.h>
#include <WiFi.h>

namespace meow
{
    typedef std::function<void(void *args, wl_status_t conn_status)> WiFiConnectHandler;

    class WiFiConnect
    {
    public:
        enum WiFiPowerLevel : uint8_t
        {
            WIFI_POWER_MIN = 0,
            WIFI_POWER_MEDIUM,
            WIFI_POWER_MAX,
        };
        WiFiConnect();
        bool tryConnectTo(String ssid, String pwd, int32_t wifi_chan, bool autoreconnect = false);
        void setOnDoneHandler(WiFiConnectHandler handler, void *args);
        void setWiFiPower(WiFiPowerLevel power_lvl);
        void disconnect();

    private:
        void *_onConnectHandlerArgs{nullptr};
        WiFiConnectHandler _onConnectHandler{nullptr};

        void callOnDoneHandler();

        static WiFiConnect *_instance;
        static void onEvent(WiFiEvent_t event);
    };
}