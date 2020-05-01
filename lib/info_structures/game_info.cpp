#include "game_info.h"

void GameInfo::Read(int socket_fd) {
  map.Read(socket_fd);
  players_info = read_vector<PlayerObjectInfo>(socket_fd);
}

void GameInfo::WriteBuffer(std::vector<char> &buffer) const {
  map.WriteBuffer(buffer);
  write_vector(buffer, players_info);
}
