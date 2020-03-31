
#include "tcp_utilities.h"
#include <cstdint>

template <class Command>
Command ReadCommand(const int fd) {
  uint32_t command;
  read_uint32(fd, &command);
  return static_cast<Command>(command);
}

template <class Command>
void WriteCommand(const Command command, const int fd) {
  auto comm = static_cast<uint32_t >(command);
  write_uint32(fd, &comm);
}

template <class Command>
void WriteBufferCommand(const Command command, std::vector<char>& buffer) {
  auto comm = static_cast<uint32_t >(command);
  serialize_uint32(buffer, comm);
}

template <class Command>
Command ReadCommandChar(const int fd) {
  uint32_t command;
  char buff[4];
  read_buffer(fd, buff, sizeof(buff));
  buff_to_uint32(buff, &command);
  return static_cast<Command>(command);
}