#pragma GCC optimize("O3")
#include "GameServer.h"
#include <WiFi.h>

namespace meow
{
    QueueHandle_t GameServer::_packet_queue;

    GameServer::GameServer()
    {
        WiFi.mode(WIFI_MODE_STA); // Виправлення помилки assert failed: tcpip_api_call (Invalid mbox)
    }

    GameServer::~GameServer()
    {
        stop();
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    bool GameServer::begin(const char *server_name, const char *pwd, bool is_local, uint8_t max_connection, uint8_t wifi_chan)
    {
        if (!server_name || !pwd)
        {
            log_e("Некоректні параметри");
            return false;
        }

        if (_server_id.isEmpty())
        {
            log_e("Не встановлено server_id");
            return false;
        }

        _max_connection = max_connection;

        if (is_local)
        {
            if (!WiFi.softAP(server_name, pwd, wifi_chan, 0, _max_connection))
                return false;

            delay(100);

            _server_ip = "http://";
            _server_ip += WiFi.softAPIP().toString();
        }
        else if (WiFi.status() != WL_CONNECTED)
        {
            log_e("Не підключено до маршрутизатора");
            return false;
        }
        else
        {
            _server_ip = "http://";
            _server_ip += WiFi.localIP().toString();
        }

        log_i("Game server address: %s", _server_ip.c_str());

        _server_name = server_name;

        _server.onPacket(onPacket, this);
        _server.listen(SERVER_PORT);

        _client_mutex = xSemaphoreCreateMutex();

        if (!_client_mutex)
        {
            log_e("Не вдалося створити _client_mutex");
            esp_restart();
        }

        _packet_queue = xQueueCreate(PACKET_QUEUE_SIZE, sizeof(UdpPacket *));

        if (!_packet_queue)
        {
            log_e("Не вдалося створити _packet_queue");
            esp_restart();
        }

        xTaskCreatePinnedToCore(pingClientTask, "pingClientTask", (1024 / 2) * 4, this, 10, &_ping_task_handler, 1);
        xTaskCreatePinnedToCore(packetHandlerTask, "packetHandlerTask", (1024 / 2) * 4, this, 10, &_packet_task_handler, 1);

        if (_ping_task_handler == NULL)
        {
            log_e("Не вдалося запустити pingClientTask");
            esp_restart();
        }

        if (_packet_task_handler == NULL)
        {
            log_e("Не вдалося запустити packetHandlerTask");
            esp_restart();
        }

        _is_freed = false;
        return true;
    }

    void GameServer::stop()
    {
        if (_is_freed)
            return;

        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        _client_data_handler = nullptr;
        _client_confirm_handler = nullptr;
        _client_disconn_handler = nullptr;

        _server.close();

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            delete it->second;

        _clients.clear();
        xSemaphoreGive(_client_mutex);

        if (_ping_task_handler)
        {
            vTaskDelete(_ping_task_handler);
            _ping_task_handler = nullptr;
        }

        if (_packet_task_handler)
        {
            vTaskDelete(_packet_task_handler);
            _packet_task_handler = nullptr;
        }

        vSemaphoreDelete(_client_mutex);

        if (_packet_queue)
        {
            vQueueDelete(_packet_queue);
            _packet_queue = nullptr;
        }

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        _is_freed = true;
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::open()
    {
        _is_open = true;
        log_i("Сервер відкрито");
    }

    void GameServer::close()
    {
        _is_open = false;
        xSemaphoreTake(_client_mutex, portMAX_DELAY);
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if (!it->second->isConfirmed())
            {
                delete it->second;
                _clients.erase(it);
            }
        }
        xSemaphoreGive(_client_mutex);
        log_i("Сервер закрито");
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::sendBroadcast(UdpPacket &packet)
    {
        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            sendPacket(it->second, packet);

        xSemaphoreGive(_client_mutex);
    }

    void GameServer::sendBroadcast(UdpPacket::Command cmd, void *data, size_t data_size)
    {
        UdpPacket pack(data_size);
        pack.setCMD(cmd);
        pack.setData(data);
        sendBroadcast(pack);
    }

    void GameServer::sendPacket(ClientWrapper *cl_wrap, UdpPacket &packet)
    {
        if (!cl_wrap)
            return;

        _server.writeTo(packet.raw(), packet.length(), cl_wrap->getIP(), cl_wrap->getPort());
    }

    void GameServer::sendPacket(IPAddress ip, UdpPacket &packet)
    {
        ClientWrapper *cl_wrap = findClient(ip);
        sendPacket(cl_wrap, packet);
    }

    void GameServer::send(IPAddress ip, UdpPacket::Command cmd, void *data, size_t data_size)
    {
        UdpPacket pack(data_size);
        pack.setCMD(cmd);
        pack.setData(data);
        sendPacket(ip, pack);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::removeClient(ClientWrapper *cl_wrap)
    {
        if (!cl_wrap)
            return;

        removeClient(cl_wrap->getIP());
    }

    void GameServer::removeClient(const char *client_name)
    {
        if (!client_name)
            return;

        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            if (it->second->hasName(client_name))
            {
                delete it->second;
                _clients.erase(it);
                break;
            }

        xSemaphoreGive(_client_mutex);
    }

    void GameServer::removeClient(IPAddress ip)
    {
        uint32_t cl_ip = ip;
        if (cl_ip == 0)
            return;

        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            if (it->first == ip)
            {
                delete it->second;
                _clients.erase(it);
                break;
            }

        xSemaphoreGive(_client_mutex);
    }

    ClientWrapper *GameServer::findClient(IPAddress ip) const
    {
        uint32_t cl_ip = ip;
        if (cl_ip == 0)
            return nullptr;

        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        auto it = _clients.find(ip);

        if (it == _clients.end())
        {
            xSemaphoreGive(_client_mutex);
            return nullptr;
        }

        xSemaphoreGive(_client_mutex);
        return it->second;
    }

    ClientWrapper *GameServer::findClient(ClientWrapper *cl_wrap) const
    {
        return findClient(cl_wrap->getIP());
    }

    ClientWrapper *GameServer::findClient(const char *name) const
    {
        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            if (it->second->hasName(name))
            {
                xSemaphoreGive(_client_mutex);
                return it->second;
            }

        xSemaphoreGive(_client_mutex);
        return nullptr;
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::sendNameRespMsg(ClientWrapper *cl_wrap, bool result)
    {
        uint8_t resp = 0;

        log_i("Авторизація:");

        if (result)
        {
            resp = 1;
            log_i("Прийнято");
        }
        else
            log_i("Відхилено");

        UdpPacket packet(1);
        packet.setCMD(UdpPacket::CMD_NAME);
        packet.setData(&resp);

        sendPacket(cl_wrap, packet);
    }

    void GameServer::sendBusyMsg(ClientWrapper *cl_wrap)
    {
        log_i("Сервер зайнятий");

        uint8_t data = 1;
        UdpPacket packet(1);
        packet.setCMD(UdpPacket::CMD_BUSY);
        packet.setData(&data);

        sendPacket(cl_wrap, packet);
        removeClient(cl_wrap);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::handleHandshake(UdpPacket *packet)
    {
        log_i("Отримано handshake");

        uint8_t result = 0;

        if (packet->isDataEquals(_server_id.c_str()))
            result = 1;

        UdpPacket resp_msg{1};
        resp_msg.setCMD(UdpPacket::CMD_HANDSHAKE);
        resp_msg.setData(&result);
        _server.writeTo(resp_msg.raw(), resp_msg.length(), packet->getRemoteIP(), packet->getRemotePort());
    }

    void GameServer::handleName(ClientWrapper *cl_wrap, UdpPacket *packet)
    {
        log_i("Запит авторизації");

        if (cl_wrap->isConfirmed())
            return;

        if (packet->dataLen() > 20 || _server_name.equals(packet->getData()) || findClient(packet->getData()))
        {
            sendNameRespMsg(cl_wrap, false);
            return;
        }

        if (_is_busy)
        {
            sendBusyMsg(cl_wrap);
            return;
        }

        _is_busy = true;

        cl_wrap->setName(packet->getData());
        callClientConfirmHandler(cl_wrap, onConfirmationResult);
    }

    void GameServer::handleData(ClientWrapper *cl_wrap, UdpPacket *packet)
    {
        if (!_client_data_handler)
            return;

        if (!cl_wrap->isConfirmed())
            removeClient(cl_wrap);
        else
            _client_data_handler(cl_wrap, packet, _client_data_arg);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::handlePacket(UdpPacket *packet)
    {
        UdpPacket::Command cmd = packet->getCMD();
        ClientWrapper *cl_wrap = findClient(packet->getRemoteIP());

        if (!cl_wrap)
        {
            if (_is_open && _clients.size() < _max_connection)
            {
                log_i("Клієнт приєднався");

                xSemaphoreTake(_client_mutex, portMAX_DELAY);
                _clients.emplace(packet->getRemoteIP(), new ClientWrapper{packet->getRemoteIP(), packet->getRemotePort()});
                xSemaphoreGive(_client_mutex);

                if (cmd == UdpPacket::CMD_HANDSHAKE)
                    handleHandshake(packet);
            }
        }
        else if (cmd == UdpPacket::CMD_PING)
        {
            cl_wrap->prolong();
        }
        else
        {
            switch (cmd)
            {
            case UdpPacket::CMD_DATA:
                handleData(cl_wrap, packet);
                break;
            case UdpPacket::CMD_NAME:
                handleName(cl_wrap, packet);
                break;
            default:
                if (CORE_DEBUG_LEVEL > 0)
                    packet->printToLog();
                break;
            }
        }
    }

    void GameServer::packetHandlerTask(void *arg)
    {
        GameServer *this_ptr = static_cast<GameServer *>(arg);
        UdpPacket *packet = nullptr;

        while (1)
        {
            if (xQueueReceive(_packet_queue, &packet, portMAX_DELAY) == pdPASS)
            {
                if (packet != nullptr)
                {
                    this_ptr->handlePacket(packet);
                    delete packet;
                    packet = nullptr;
                }
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

    void GameServer::onPacket(void *arg, AsyncUDPPacket &packet)
    {
        if (packet.length() > 1000 || packet.length() == 0)
        {
            log_e("Некоректний пакет. Size: %zu", packet.length());
            return;
        }

        UdpPacket *pack = new UdpPacket(packet);

        if (!xQueueSend(_packet_queue, &pack, portMAX_DELAY) == pdPASS)
        {
            log_e("Черга _packet_queue переповнена");
            delete pack;
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::handlePingClient()
    {
        xSemaphoreTake(_client_mutex, portMAX_DELAY);

        UdpPacket ping(1);
        ping.setCMD(UdpPacket::CMD_PING);

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if (!it->second->isConnected())
            {
                log_i("Клієнт від'єднався");

                if (it->second->isConfirmed())
                {
                    --_cur_clients_size;
                    callDisconnHandler(it->second);
                }
                delete it->second;
                _clients.erase(it);
            }
            else
                sendPacket(it->second, ping);
        }
        xSemaphoreGive(_client_mutex);
    }

    void GameServer::pingClientTask(void *arg)
    {
        GameServer *this_ptr = static_cast<GameServer *>(arg);

        while (1)
        {
            this_ptr->handlePingClient();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    void GameServer::handleNameConfirm(ClientWrapper *cl_wrap, bool result)
    {
        _is_busy = false;

        if (findClient(cl_wrap))
        {
            cl_wrap->confirm();

            sendNameRespMsg(cl_wrap, result);

            if (!result)
                removeClient(cl_wrap);
            else
                ++_cur_clients_size;
        }
    }

    void GameServer::onConfirmationResult(ClientWrapper *cl_wrap, bool result, GameServer *server_ptr)
    {
        server_ptr->handleNameConfirm(cl_wrap, result);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameServer::callClientConfirmHandler(ClientWrapper *cl_wrap, ConfirmResultHandler result_handler)
    {
        if (!_client_confirm_handler)
        {
            sendNameRespMsg(cl_wrap, false);
            _is_busy = false;
            return;
        }

        _client_confirm_handler(cl_wrap, result_handler, _client_confirm_arg);
    }

    void GameServer::callDisconnHandler(ClientWrapper *cl_wrap)
    {
        if (_client_disconn_handler)
            _client_disconn_handler(cl_wrap, _client_disconn_arg);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

#pragma region set_handler

    void GameServer::onConfirmation(ClientConfirmHandler handler, void *arg)
    {
        _client_confirm_handler = handler;
        _client_confirm_arg = arg;
    }

    void GameServer::onDisconnect(ClientDisconnHandler handler, void *arg)
    {
        _client_disconn_handler = handler;
        _client_disconn_arg = arg;
    }

    void GameServer::onData(ClientDataHandler handler, void *arg)
    {
        _client_data_handler = handler;
        _client_data_arg = arg;
    }

#pragma endregion set_handler
}
