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
        bool begin(const char *server_name, const char *pwd, bool is_local = true, uint8_t max_connection = 1, uint8_t wifi_chan = 6);
        void stop();
        //
        void open();
        void close();
        bool isOpen() const { return _is_open; }
        bool isFull() const { return _max_connection == _cur_clients_size; }
        //
        void removeClient(ClientWrapper *cl_wrap);
        void removeClient(const char *client_name);
        void removeClient(IPAddress ip);
        void sendBroadcast(UdpPacket &packet);
        void sendBroadcast(UdpPacket::Command cmd, void *data, size_t data_size);
        void sendPacket(ClientWrapper *cl_wrap, UdpPacket &packet);
        void sendPacket(IPAddress ip, UdpPacket &packet);
        void send(IPAddress ip, UdpPacket::Command cmd, void *data, size_t data_size);
        //
        void onConfirmation(ClientConfirmHandler handler, void *arg);
        void onDisconnect(ClientDisconnHandler handler, void *arg);
        void onData(ClientDataHandler handler, void *arg);

        const std::unordered_map<uint32_t, ClientWrapper *> *getClients() { return &_clients; }
        const char *getServerIP() const { return _server_ip.c_str(); }

        const char *getName() const { return _server_name.c_str(); }

    protected:
        static const uint16_t SERVER_PORT = 777;
        static const uint16_t PACKET_QUEUE_SIZE = 30;

        bool _is_freed{true};

        uint8_t _max_connection;
        uint8_t _cur_clients_size{0};

        TaskHandle_t _ping_task_handler{nullptr};
        TaskHandle_t _packet_task_handler{nullptr};
        static QueueHandle_t _packet_queue;
        SemaphoreHandle_t _client_mutex{nullptr};
        SemaphoreHandle_t _udp_mutex{nullptr};

        String _server_name;
        String _server_id;
        String _server_ip;
        bool _is_open{false};

        AsyncUDP _server;
        std::unordered_map<uint32_t, ClientWrapper *> _clients;
        //
        ClientConfirmHandler _client_confirm_handler{nullptr};
        void *_client_confirm_arg{nullptr};
        ClientDisconnHandler _client_disconn_handler{nullptr};
        void *_client_disconn_arg{nullptr};
        ClientDataHandler _client_data_handler{nullptr};
        void *_client_data_arg{nullptr};

        bool _is_busy{false};
        //
        ClientWrapper *findClient(IPAddress ip) const;
        ClientWrapper *findClient(ClientWrapper *cl_wrap) const;
        ClientWrapper *findClient(const char *name) const;
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