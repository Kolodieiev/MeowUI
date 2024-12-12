#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <list>
#include <vector>
#include "../AsyncTCP.h"
#include "./AsyncClientWrapper.h"
#include "../TcpCMD.h"

namespace meow
{
    typedef std::function<void(bool result, AsyncClient *, void *server)> ConfirmResultHandler;
    typedef std::function<void(String client_name, AsyncClient *client, ConfirmResultHandler result_handler, void *arg)> ClientConfirmHandler;
    typedef std::function<void(uint32_t client_addr, void *arg)> ClientDisconnectHandler;
    typedef std::function<void(uint32_t client_addr, void *data, size_t len, void *arg)> ClientActionHandler;

    class GameServer
    {
    public:
        GameServer();
        ~GameServer();
        bool begin(const char *ssid = "meow", const char *pwd = "12345678", uint8_t wifi_chan = 9, uint8_t max_client = 2);
        void stop();
        //
        void openLobby() { _in_lobby = true; }
        void closeLobby();
        std::vector<String> getClientsName();
        void removeClientByAddr(uint32_t addr);
        //
        void sendBroadcast(const char *data, size_t len);
        void sendMessage(uint32_t client_addr, const char *data, size_t len);
        //
        void setClientConfirmHandler(ClientConfirmHandler handler, void *arg);
        void setClientDisconnHandler(ClientDisconnectHandler handler, void *arg);
        void setGameActionHandler(ClientActionHandler handler, void *arg);

    private:
        bool _in_lobby{false};
        AsyncServer _server{80};
        std::list<AsyncClientWrapper *> _clients;
        //
        ClientConfirmHandler _client_confirm_handler{nullptr};
        void *_client_confirm_arg{nullptr};
        ClientDisconnectHandler _client_disconn_handler{nullptr};
        void *_client_disconn_arg{nullptr};
        ClientActionHandler _client_action_handler{nullptr};
        void *_client_action_arg{nullptr};

        bool _is_busy{false};
        //
        void clientHandler(AsyncClient *client);
        void disconnectHandler(AsyncClient *client);
        void dataHandler(AsyncClient *client, void *data, size_t len);
        void ackHandler(AsyncClient *client, size_t len, uint32_t time);
        void sendToClient(AsyncClientWrapper *client_wrap, const char *data, size_t len);
        void clientConfirmHandler(bool result, AsyncClient *client);
        //
        void setClientName(AsyncClient *client, void *data, size_t len);
        void callClientActionHandler(AsyncClient *client, void *data, size_t len);
        void callClientConfirmHandler(String client_name, AsyncClientWrapper *client_wrap, ConfirmResultHandler result_handler);
        void printData(void *data, size_t len);
        //
        void removeClient(AsyncClient *client);
        AsyncClientWrapper *findClient(AsyncClient *client);
        bool hasClientWithName(const char *name);
        char *createPacket(TcpCMD cmd, void *data, size_t len);
        void sendConfirmMsg(AsyncClientWrapper *client_wrap);
        void sendBusyMsg(AsyncClientWrapper *client_wrap);
        //
        void freeResources();
        //
        static void onClient(void *arg, AsyncClient *client);
        static void onDisconnect(void *arg, AsyncClient *client);
        static void onData(void *arg, AsyncClient *client, void *data, size_t len);
        static void onAck(void *arg, AsyncClient *client, size_t len, uint32_t time);
        static void onConfirmationResult(bool result, AsyncClient *client, void *server);
    };
}