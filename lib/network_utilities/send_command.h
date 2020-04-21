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

// Command for players that waiting in lobby
enum CommandToPlayer {
  START_GAME,
  UPDATE_PLAYERS
};

// Command from host to room
enum CommandToRoom {
  START_GAME_HOST
};

// Command to game
enum CommandToGame {
  UNKNOWN_INPUT,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  SHOOT
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
void WriteBufferCommand(const Command command, std::vector<char>& buffer);