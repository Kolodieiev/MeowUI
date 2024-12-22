#pragma GCC optimize("O3")
#include "GameClient.h"
#include <WiFi.h>

namespace meow
{
    QueueHandle_t GameClient::_packet_queue;

    GameClient::GameClient()
    {
        WiFi.mode(WIFI_MODE_STA); // Виправлення помилки assert failed: tcpip_api_call (Invalid mbox)
    }

    GameClient::~GameClient()
    {
        disconnect();
    }

    bool GameClient::connect(const char *host_ip)
    {
        if (_name.isEmpty())
        {
            log_e("Не вказано ім'я клієнта");
            return false;
        }

        if (_server_id.isEmpty())
        {
            log_e("Не вказано id сервера");
            return false;
        }

        if (!_server_ip.fromString(host_ip))
        {
            log_e("Некоректний IP сервера: %s", host_ip);
            return false;
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            log_e("Не підключено до маршрутизатора");
            return false;
        }

        log_i("Приєднання до сервера...");
        _status = STATUS_IDLE;

        _client.onPacket(onPacket, this);
        _client.listen(CLIENT_PORT);

        _last_act_time = millis();

        _packet_queue = xQueueCreate(PACKET_QUEUE_SIZE, sizeof(UdpPacket *));

        if (!_packet_queue)
        {
            log_e("Не вдалося створити _packet_queue");
            esp_restart();
        }

        xTaskCreatePinnedToCore(checkConnectTask, "checkConnectTask", (1024 / 2) * 4, this, 10, &_check_task_handler, 1);
        xTaskCreatePinnedToCore(packetHandlerTask, "packetHandlerTask", (1024 / 2) * 4, this, 10, &_packet_task_handler, 1);

        if (_check_task_handler == NULL)
        {
            log_e("Не вдалося запустити checkConnectTask");
            esp_restart();
        }

        if (_packet_task_handler == NULL)
        {
            log_e("Не вдалося запустити packetHandlerTask");
            esp_restart();
        }

        sendHandshake();

        _is_freed = false;
        return true;
    }

    void GameClient::disconnect()
    {
        if (_is_freed)
            return;

        log_i("Від'єднано від сервера");

        _client.close();

        if (_check_task_handler)
            vTaskDelete(_check_task_handler);
        if (_packet_task_handler)
            vTaskDelete(_packet_task_handler);
        if (_packet_queue)
            vQueueDelete(_packet_queue);

        _is_freed = true;
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameClient::sendPacket(UdpPacket &packet)
    {
        if (_status != STATUS_CONNECTED && _status != STATUS_IDLE)
        {
            log_e("Некоректний стан: %d", _status);
            return;
        }

        _client.sendTo(packet, _server_ip, SERVER_PORT);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameClient::sendHandshake()
    {
        log_i("Розпізнавання...");

        UdpPacket packet(_server_id.length());
        packet.setCommand(UdpPacket::CMD_HANDSHAKE);
        packet.setData((void *)_server_id.c_str());

        sendPacket(packet);
    }

    void GameClient::sendName()
    {
        log_i("Авторизація...");

        UdpPacket packet(_name.length());
        packet.setCommand(UdpPacket::CMD_NAME);
        packet.setData((void *)_name.c_str());

        sendPacket(packet);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameClient::handlePacket(UdpPacket *packet)
    {
        switch (packet->getCommand())
        {
        case UdpPacket::CMD_DATA:
            callDataHandler(packet);
            break;
        case UdpPacket::CMD_PING:
            handlePing();
            break;
        case UdpPacket::CMD_NAME:
            handleNameConfirm(packet);
            break;
        case UdpPacket::CMD_HANDSHAKE:
            handleHandshake(packet);
            break;
        default:
            if (CORE_DEBUG_LEVEL > 0)
                packet->printToLog();
            break;
        }
    }

    void GameClient::packetHandlerTask(void *arg)
    {
        GameClient *this_ptr = static_cast<GameClient *>(arg);
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

    void GameClient::onPacket(void *arg, AsyncUDPPacket &packet)
    {
        if (packet.length() > 100 || packet.length() == 0)
        {
            log_e("Некоректний пакет");
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

    void GameClient::handleHandshake(UdpPacket *packet)
    {
        if (static_cast<uint8_t>(packet->getData()[0]) != 1)
        {
            log_e("Некоректний сервер");
            _status = STATUS_WRONG_SERVER;
            callDisconnHandler();
            disconnect();
        }
        else
        {
            log_i("Сервер розпізнано");
            sendName();
        }
    }

    void GameClient::handleNameConfirm(UdpPacket *packet)
    {
        if (static_cast<uint8_t>(packet->getData()[0]) != 1)
        {
            log_e("Помилка авторизації");
            _status = STATUS_WRONG_NAME;
            callDisconnHandler();
            disconnect();
        }
        else
        {
            log_i("Авторизація успішна");
            _status = STATUS_CONNECTED;
            callConnectHandler();
        }
    }

    void GameClient::handlePing()
    {
        _last_act_time = millis();

        UdpPacket packet(1);
        packet.setCommand(UdpPacket::CMD_PING);

        sendPacket(packet);
    }

    // ------------------------------------------------------------------------------------------------------------------------------

    void GameClient::handleCheckConnect()
    {
        if (millis() - _last_act_time > 3000)
        {
            log_i("onTimeout");
            _status = STATUS_DISCONNECTED;
            callDisconnHandler();
            disconnect();
        }
    }

    void GameClient::checkConnectTask(void *arg)
    {
        GameClient *this_ptr = static_cast<GameClient *>(arg);

        while (1)
        {
            this_ptr->handleCheckConnect();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------

#pragma region call_handler

    void GameClient::callDataHandler(UdpPacket *packet)
    {
        if (_server_data_handler)
        {
            log_i("Викликаю data_handler");
            _server_data_handler(packet, _server_data_arg);
        }
    }

    void GameClient::callConnectHandler()
    {
        if (_server_connected_handler)
        {
            log_i("Викликаю connected_handler");
            _server_connected_handler(_server_connected_arg);
        }
    }

    void GameClient::callDisconnHandler()
    {
        if (_server_disconn_handler)
        {
            log_i("Викликаю disconn_handler");
            _server_disconn_handler(_server_disconn_arg);
        }
    }

#pragma endregion call_handler

    // ------------------------------------------------------------------------------------------------------------------------------

#pragma region set_handler

    void GameClient::setDataHandler(ServerDataHandler data_handler, void *arg)
    {
        _server_data_handler = data_handler;
        _server_data_arg = arg;
    }

    void GameClient::setConnectHandler(ServerConnectedHandler conn_handler, void *arg)
    {
        _server_connected_handler = conn_handler;
        _server_connected_arg = arg;
    }

    void GameClient::setDisconnHandler(ServerDisconnHandler disconn_handler, void *arg)
    {
        _server_disconn_handler = disconn_handler;
        _server_disconn_arg = arg;
    }

#pragma endregion set_handler
} // namespace meow
