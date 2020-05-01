#include "game_map_info.h"

void GameMapInfo::Read(const int socket_fd) {
  uint32_t size;
  read_uint32_by_char(socket_fd, &size);
  map.resize(size);
  for (int i = 0; i < size; ++i) {
    std::string s = read_string(socket_fd);
    map[i] = std::vector<char>(s.begin(), s.end());
  }
}

void GameMapInfo::WriteBuffer(std::vector<char> &buffer) const {
  serialize_uint32(buffer, map.size());
  for (int i = 0; i < map.size(); ++i) {
    std::string s(map[i].begin(), map[i].end());
    serialize_string(buffer, s);
  }
}