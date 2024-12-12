#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <vector>

namespace meow
{
    typedef std::function<void(void *)> WiFiScanDoneHandler;

    class WiFiScanner
    {
    public:
        bool startScan();
        std::vector<String> getScanResult() const;
        void setOnDoneHandler(WiFiScanDoneHandler handler, void *args);

    private:
        void *_onDoneHandlerArgs{nullptr};
        WiFiScanDoneHandler _onDoneHandler{nullptr};

        void callOnDoneHandler();

        static void waitFinishTask(void *params);
    };
}