#include "player_object_info.h"

void PlayerObjectInfo::Read(const int socket_fd) {
  read_uint32_by_char(socket_fd, &id);
  read_uint32_by_char(socket_fd, &health_value);
}

void PlayerObjectInfo::WriteBuffer(std::vector<char> &buffer) const {
  serialize_uint32(buffer, id);
  serialize_uint32(buffer, health_value);
}
