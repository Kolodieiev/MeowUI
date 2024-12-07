#pragma once
#include <stdint.h>

namespace meow
{
    enum TcpCMD : uint8_t
    {
        TCP_CMD_NAME = 0,
        TCP_CMD_DISCONN,
        TCP_CMD_GAME_ACT,
        TCP_CMD_BUSY,
        TCP_CMD_WAIT,
        TCP_CMD_CONFIRM
    };
}