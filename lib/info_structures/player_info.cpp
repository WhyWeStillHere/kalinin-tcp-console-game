#include "player_info.h"
#include "../network_utilities/tcp_utilities.h"

void PlayerInfo::Read(const int socket_fd) {
  read_uint32_by_char(socket_fd, &id);
  login = read_string(socket_fd);
}

void PlayerInfo::WriteBuffer(std::vector<char> &buffer) const {
  serialize_uint32(buffer, id);
  serialize_string(buffer, login);
}