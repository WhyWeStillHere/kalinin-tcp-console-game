#pragma once

#include <vector>
#include "send_command_impl.h"

// Commands that server manager is wait for
enum CommandToManager {
  REFRESH_ROOMS,
  JOIN_ROOM,
  CREATE_ROOM
};

// Commands that client is wait for
enum CommandToClient {
  ERROR,
  ROOM_CREATED,
};

// Use if someone send commands like uin32_t
template <class Command>
Command ReadCommand(const int fd);

// Use if someone send commands like 4 chars
template <class Command>
Command ReadCommandChar(const int fd);

// Use then you send command like uin32_t
template <class Command>
void WriteCommand(const Command command, const int fd);

// Use then you split your command on 4 chars and
// write it to buffer
template <class Command>
void WriteBufferCommand(const Command command, const
    int fd, std::vector<char>& buffer);