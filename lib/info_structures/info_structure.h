#pragma once

#include "../network_utilities/tcp_utilities.h"
#include <iostream>
#include <vector>

class InfoStructure {
public:
  virtual void Read(int socket_fd) = 0;
  virtual void WriteBuffer(std::vector<char>& buffer) const = 0;
};

template <typename InfoStruct>
std::vector<InfoStruct> read_vector(const int socket_fd) {
  std::vector<InfoStruct> structs_info;
  uint32_t vect_size;
  read_uint32_by_char(socket_fd, &vect_size);
  structs_info.resize(vect_size);
  for (auto& structure: structs_info) {
    structure.Read(socket_fd);
  }
  return structs_info;
}

template <typename InfoStruct>
void write_vector(std::vector<char>& buffer,
    std::vector<InfoStruct> structs_info) {
  uint32_t vect_size = structs_info.size();
  serialize_uint32(buffer, vect_size);
  for (auto& structure: structs_info) {
    structure.WriteBuffer(buffer);
  }
}

