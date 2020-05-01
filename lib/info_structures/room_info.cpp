#include <unistd.h>
#include <stdexcept>

#include "../network_utilities/tcp_utilities.h"
#include "room_info.h"

void RoomInfo::Read(const int socket_fd) {
  char buffer[8];
  read_buffer(socket_fd, buffer, 8);

  char* buffer_ind = buffer;
  buffer_ind = buff_to_uint32(buffer_ind, &id);
  buffer_ind = buff_to_uint32(buffer_ind, &participant_num);
}

void RoomInfo::WriteBuffer(std::vector<char> &buffer) const {
  serialize_uint32(buffer, id);
  serialize_uint32(buffer, participant_num);
}