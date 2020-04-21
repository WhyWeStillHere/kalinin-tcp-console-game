#pragma once
#include "info_structure.h"

class PlayerObjectInfo: public InfoStructure {
public:
  uint32_t id;
  uint32_t health_value;
  uint32_t x_coord;
  uint32_t y_coord;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override ;
};
