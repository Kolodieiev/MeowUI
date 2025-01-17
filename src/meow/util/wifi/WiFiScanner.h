#pragma once
#include <Arduino.h>
#include <WiFi.h>

#include <vector>

namespace meow
{
    typedef std::function<void(void * arg)> WiFiScanDoneHandler;

    class WiFiScanner
    {
    public:
        WiFiScanner();

        /**
         * @brief Налашттовує WiFi модуль та запускає сканування точок доступу.
         *
         * @return true - якщо сканування було запущено.
         * @return false - якщо сканування не було запущено.
         */
        bool startScan();

        /**
         * @brief Наповнює вектор іменами всіх виявлених точок доступу.
         * Після формування вектора, результат сканування очищується.
         *
         */
        void getScanResult(std::vector<String> &out_vector) const;

        /**
         * @brief Встановлює обробник події, який буде викликано після завершення сканування.
         *
         * @param handler Асинхронний обробник події завершення сканування.
         * @param args Аргументи, що будуть передані обробнику.
         */
        void setOnDoneHandler(WiFiScanDoneHandler handler, void *args);

    private:
        void *_onDoneHandlerArgs{nullptr};
        WiFiScanDoneHandler _onDoneHandler{nullptr};

        void callOnDoneHandler();

        static WiFiScanner *_instance;
        static void onEvent(WiFiEvent_t event);
    };
}