#pragma GCC optimize("O3")
#include "GameServer.h"
#include <cstring>
#include <WiFi.h>

namespace meow
{

    GameServer::GameServer()
    {
    }

    GameServer::~GameServer()
    {
        freeResources();
    }

    bool GameServer::begin(const char *ssid, const char *pwd, uint8_t wifi_chan, uint8_t max_client)
    {
        if (!WiFi.softAP(ssid, pwd, wifi_chan, 0, 4))
            return false;

        delay(10);

        String server_addr;

        IPAddress IP = WiFi.softAPIP();
        server_addr = "http://";
        server_addr += IP.toString();

        log_i("Game server address: %s", server_addr.c_str());

        _server.setNoDelay(true);
        _server.begin();
        _server.onClient(onClient, this);

        return true;
    }

    void GameServer::stop()
    {
        freeResources();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    void GameServer::closeLobby()
    {
        _in_lobby = false;

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if (std::strcmp((*it)->getName(), "") == 0)
            {
                delete *it;
                _clients.erase(it);
            }
        }
    }

    void GameServer::sendBroadcast(const char *data, size_t len)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            sendToClient(*it, data, len);
    }

    void GameServer::sendMessage(uint32_t client_addr, const char *data, size_t len)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if ((*it)->getAddress() == client_addr)
                sendToClient(*it, data, len);
        }
    }

    void GameServer::sendToClient(AsyncClientWrapper *client_wrap, const char *data, size_t len)
    {
        AsyncClient *rec = client_wrap->getClient();
        if (client_wrap->canSend())
        {
            rec->add(data, len);
            rec->send();
            client_wrap->setSendTS();
        }
        else if (client_wrap->isDisconnected())
        {
            log_i("Client %s unavailable", client_wrap->getName());
            disconnectHandler(rec);
        }
    }

    void GameServer::clientConfirmHandler(bool result, AsyncClient *client)
    {
        if (!result)
            removeClient(client);
        else
        {
            AsyncClientWrapper *client_wrapper = findClient(client);

            if (client_wrapper)
                sendConfirmMsg(client_wrapper);
        }

        _is_busy = false;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region "static"

    void GameServer::onClient(void *arg, AsyncClient *client)
    {
        GameServer *server = static_cast<GameServer *>(arg);
        server->clientHandler(client);
    }

    void GameServer::onDisconnect(void *arg, AsyncClient *client)
    {
        GameServer *server = static_cast<GameServer *>(arg);
        server->disconnectHandler(client);
    }

    void GameServer::onData(void *arg, AsyncClient *client, void *data, size_t len)
    {
        GameServer *server = static_cast<GameServer *>(arg);
        server->dataHandler(client, data, len);
    }

    void GameServer::onAck(void *arg, AsyncClient *client, size_t len, uint32_t time)
    {
        GameServer *server = static_cast<GameServer *>(arg);
        server->ackHandler(client, len, time);
    }

    void GameServer::onConfirmationResult(bool result, AsyncClient *client, void *server)
    {
        GameServer *g_server = static_cast<GameServer *>(server);
        g_server->clientConfirmHandler(result, client);
    }

#pragma endregion "static"

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    void GameServer::clientHandler(AsyncClient *client)
    {
        if (!_in_lobby)
        {
            client->close();
            return;
        }

        removeClient(client);
        AsyncClientWrapper *g_client = new AsyncClientWrapper(client);
        _clients.push_back(g_client);
        client->onDisconnect(onDisconnect, this);
        client->onData(onData, this);
        client->onAck(onAck, this);
    }

    void GameServer::disconnectHandler(AsyncClient *client)
    {
        AsyncClientWrapper *cl_wrap = findClient(client);
        if (!cl_wrap)
            return;

        if (_client_disconn_handler && cl_wrap->isConfirmed())
            _client_disconn_handler(client->getRemoteAddress(), _client_disconn_arg);

        cl_wrap->markFreed();

        removeClient(client);
    }

    void GameServer::dataHandler(AsyncClient *client, void *data, size_t len)
    {
        switch (((uint8_t *)data)[0])
        {
        case TCP_CMD_NAME:
            setClientName(client, data, len);
            break;
        case TCP_CMD_DISCONN:
            disconnectHandler(client);
            break;
        case TCP_CMD_GAME_ACT:
            callClientActionHandler(client, data, len);
            break;
        default:
            printData(data, len);
            break;
        }
    }

    void GameServer::ackHandler(AsyncClient *client, size_t len, uint32_t time)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if ((*it)->getAddress() == client->getRemoteAddress())
            {
                (*it)->sendComplete();
                return;
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void GameServer::removeClient(AsyncClient *client)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if ((*it)->is(client))
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
    }

    AsyncClientWrapper *GameServer::findClient(AsyncClient *client)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if ((*it)->is(client))
                return (*it);
        }

        return nullptr;
    }

    bool GameServer::hasClientWithName(const char *name)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if (std::strcmp((*it)->getName(), name) == 0)
                return true;
        }

        return false;
    }

    char *GameServer::createPacket(TcpCMD cmd, void *data, size_t len)
    {
        char *packet = nullptr;

        try
        {
            packet = new char[len + 1];
            packet[0] = cmd;
            std::memcpy(&packet[1], static_cast<char *>(data), len);
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }

        return packet;
    }

    void GameServer::sendConfirmMsg(AsyncClientWrapper *client_wrap)
    {
        client_wrap->confirm();

        String msg = client_wrap->getName();

        char *packet = createPacket(TCP_CMD_CONFIRM, (void *)msg.c_str(), msg.length());
        sendToClient(client_wrap, packet, msg.length() + 1);
        delete packet;

        log_i("Client %s added", msg.c_str());
    }

    void GameServer::sendBusyMsg(AsyncClientWrapper *client_wrap)
    {
        String msg = "";

        char *packet = createPacket(TCP_CMD_BUSY, (void *)msg.c_str(), msg.length());
        sendToClient(client_wrap, packet, msg.length() + 1);
        removeClient(client_wrap->getClient());
        delete packet;
    }

    void GameServer::freeResources()
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
            delete *it;

        _clients.clear();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    std::vector<String> GameServer::getClientsName()
    {
        std::vector<String> names;
        names.reserve(_clients.size());

        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if (std::strcmp((*it)->getName(), "") != 0)
                names.push_back((*it)->getName());
        }

        return names;
    }

    void GameServer::removeClientByAddr(uint32_t addr)
    {
        for (auto it = _clients.begin(), last_it = _clients.end(); it != last_it; ++it)
        {
            if ((*it)->getAddress() == addr)
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
    }

    void GameServer::setClientName(AsyncClient *client, void *data, size_t len)
    {
        AsyncClientWrapper *client_wrap = findClient(client);
        if (!client_wrap)
            return;

        if (_is_busy)
        {
            sendBusyMsg(client_wrap);
            return;
        }

        char name_data[len - 1];
        std::memcpy(name_data, &(static_cast<char *>(data))[1], len - 1);
        name_data[len - 1] = '\0';

        if (std::strcmp(name_data, "") == 0 || hasClientWithName(name_data))
        {
            removeClient(client);
            return;
        }

        client_wrap->setName(name_data);

        callClientConfirmHandler(String(name_data), client_wrap, onConfirmationResult);
    }

    void GameServer::callClientActionHandler(AsyncClient *client, void *data, size_t len)
    {
        if (!_client_action_handler)
            return;

        AsyncClientWrapper *cl_wrap = findClient(client);

        if (!cl_wrap)
            return;

        if (cl_wrap->isConfirmed())
            _client_action_handler(client->getRemoteAddress(), data, len, _client_action_arg);
        else
            removeClient(client);
    }

    void GameServer::callClientConfirmHandler(String client_name, AsyncClientWrapper *client_wrap, ConfirmResultHandler result_handler)
    {
        _is_busy = true;

        if (!_client_confirm_handler)
        {
            sendConfirmMsg(client_wrap);

            _is_busy = false;
            return;
        }

        _client_confirm_handler(client_name, client_wrap->getClient(), result_handler, _client_confirm_arg);
    }

    void GameServer::printData(void *data, size_t len)
    {
        char ch_data[len + 1];

        std::memcpy(ch_data, static_cast<char *>(data), len);
        ch_data[len] = '\0';

        log_i("Handled data %s", ch_data);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    void GameServer::setClientConfirmHandler(ClientConfirmHandler handler, void *arg)
    {
        _client_confirm_handler = handler;
        _client_confirm_arg = arg;
    }

    void GameServer::setClientDisconnHandler(ClientDisconnectHandler handler, void *arg)
    {
        _client_disconn_handler = handler;
        _client_disconn_arg = arg;
    }

    void GameServer::setGameActionHandler(ClientActionHandler handler, void *arg)
    {
        _client_action_handler = handler;
        _client_action_arg = arg;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------
}