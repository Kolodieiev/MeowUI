#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>
#include <unordered_map>

#include "UdpPacket.h"
#include "ClientWrapper.h"

namespace meow
{
    class GameServer;

    typedef std::function<void(ClientWrapper *client_wrap, bool result, GameServer *server_ptr)> ConfirmResultHandler;
    typedef std::function<void(ClientWrapper *client_wrap, ConfirmResultHandler result_handler, void *arg)> ClientConfirmHandler;
    typedef std::function<void(ClientWrapper *client_wrap, void *arg)> ClientDisconnHandler;
    typedef std::function<void(ClientWrapper *client_wrap, UdpPacket *packet, void *arg)> ClientDataHandler;

    class GameServer
    {

    public:
        GameServer();
        ~GameServer();

        void setServerID(const char *id) { _server_id = id; };
        bool begin(const char *ssid, const char *pwd, bool is_local = true, uint8_t max_connection = 1, uint8_t wifi_chan = 6);
        void stop();
        //
        void openLobby() { _in_lobby = true; }
        void closeLobby();
        //
        void removeClient(ClientWrapper *cl_wrap);
        void sendBroadcast(UdpPacket &packet);
        void sendPacket(ClientWrapper *cl_wrap, UdpPacket &packet);
        //
        void setConfirmHandler(ClientConfirmHandler handler, void *arg);
        void setDisconnHandler(ClientDisconnHandler handler, void *arg);
        void setDataHandler(ClientDataHandler handler, void *arg);

        const std::unordered_map<uint32_t, ClientWrapper *> *getClients() { return &_clients; }
        const char *getServerIP() const { return _server_ip.c_str(); }

    private:
        static const uint16_t SERVER_PORT = 777;
        static const uint16_t PACKET_QUEUE_SIZE = 30;

        bool _is_freed{true};

        uint8_t _max_connection;
        uint8_t _cur_clients_size{0};

        TaskHandle_t _ping_task_handler = NULL;
        TaskHandle_t _packet_task_handler = NULL;
        static QueueHandle_t _packet_queue;
        SemaphoreHandle_t _client_mutex;

        String _server_id;
        String _server_ip;
        bool _in_lobby{false};

        AsyncUDP _server;
        std::unordered_map<uint32_t, ClientWrapper *> _clients;
        //
        ClientConfirmHandler _client_confirm_handler{nullptr};
        void *_client_confirm_arg{nullptr};
        ClientDisconnHandler _client_disconn_handler{nullptr};
        void *_client_disconn_arg{nullptr};
        ClientDataHandler _server_data_handler{nullptr};
        void *_server_data_arg{nullptr};

        bool _is_busy{false};
        //
        ClientWrapper *findClient(IPAddress ip) const;
        ClientWrapper *findClient(ClientWrapper *cl_wrap) const;
        ClientWrapper *findClientByName(const char *name) const;
        //
        void handlePacket(UdpPacket *packet);
        static void packetHandlerTask(void *arg);
        //
        static void onPacket(void *arg, AsyncUDPPacket &packet);
        //
        void handleHandshake(UdpPacket *packet);
        void handleName(ClientWrapper *cl_wrap, UdpPacket *packet);
        void handleData(ClientWrapper *cl_wrap, UdpPacket *packet);
        //
        void sendNameRespMsg(ClientWrapper *cl_wrap, bool result);
        void sendBusyMsg(ClientWrapper *cl_wrap);
        //
        void callDisconnHandler(ClientWrapper *cl_wrap);
        void callClientConfirmHandler(ClientWrapper *cl_wrap, ConfirmResultHandler result_handler);
        //
        void handlePingClient();
        static void pingClientTask(void *arg);
        //
        void handleNameConfirm(ClientWrapper *cl_wrap, bool result);
        static void onConfirmationResult(ClientWrapper *cl_wrap, bool result, GameServer *server_ptr);
        //
    };
}