#include "player_info.h"
#include "tcp_utilities.h"

void PlayerInfo::Read(const int socket_fd) {
  login = read_string(socket_fd);
}

void PlayerInfo::WriteBuffer(std::vector<char> &buffer) {
  serialize_string(buffer, login);
}