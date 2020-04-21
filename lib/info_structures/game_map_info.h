#pragma once
#include "info_structure.h"

class GameMapInfo: public InfoStructure {
public:
  std::vector< std::vector<char> > map;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override ;
};
