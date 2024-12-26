#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>
#include "UdpPacket.h"

namespace meow
{
    typedef std::function<void(void *arg)> ServerDisconnHandler;
    typedef std::function<void(void *arg)> ServerConnectedHandler;
    typedef std::function<void(UdpPacket *packet, void *arg)> ServerDataHandler;

    class GameClient
    {
    public:
        GameClient();
        ~GameClient();

        enum ClientStatus : uint8_t
        {
            STATUS_IDLE = 0,
            STATUS_CONNECTED,
            STATUS_DISCONNECTED,
            STATUS_WRONG_SERVER,
            STATUS_WRONG_NAME,
            STATUS_SERVER_BUSY,
        };

        void setName(const char *name) { _name = name; }
        const char *getName() const { return _name.c_str(); }
        void setServerID(const char *id) { _server_id = id; }
        bool connect(const char *host_ip = "192.168.4.1");
        void disconnect();
        void sendPacket(UdpPacket &packet);
        void send(UdpPacket::Command cmd, void *data, size_t data_size);

        ClientStatus getStatus() const { return _status; }
        //
        void onData(ServerDataHandler data_handler, void *arg);
        void onConnect(ServerConnectedHandler conn_handler, void *arg);
        void onDisconnect(ServerDisconnHandler disconn_handler, void *arg);

    protected:
        static const uint16_t SERVER_PORT = 777;
        static const uint16_t CLIENT_PORT = 777;
        static const uint16_t PACKET_QUEUE_SIZE = 6;

        bool _is_freed{true};

        String _name;
        String _server_id;
        IPAddress _server_ip;
        AsyncUDP _client;
        ClientStatus _status{STATUS_DISCONNECTED};
        TaskHandle_t _check_task_handler = NULL;
        TaskHandle_t _packet_task_handler = NULL;
        SemaphoreHandle_t _udp_mutex;
        static QueueHandle_t _packet_queue;
        //
        unsigned long _last_act_time;
        //
        ServerConnectedHandler _server_connected_handler{nullptr};
        void *_server_connected_arg{nullptr};

        ServerDisconnHandler _server_disconn_handler{nullptr};
        void *_server_disconn_arg{nullptr};

        ServerDataHandler _server_data_handler{nullptr};
        void *_server_data_arg{nullptr};
        //
        void sendHandshake();
        void sendName();
        //
        void handlePacket(UdpPacket *packet);
        static void packetHandlerTask(void *arg);
        //
        static void onPacket(void *arg, AsyncUDPPacket &packet);
        //
        void handleCheckConnect();
        static void checkConnectTask(void *arg);
        //
        void handleHandshake(UdpPacket *packet);
        void handleNameConfirm(UdpPacket *packet);
        void handlePing();
        //
        void callDataHandler(UdpPacket *packet);
        void callConnectHandler();
        void callDisconnHandler();
    };
} // namespace meow
