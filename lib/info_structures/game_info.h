#pragma once
#include "game_map_info.h"
#include "info_structure.h"
#include "player_object_info.h"

class GameInfo: public InfoStructure {
public:
  GameMapInfo map;
  std::vector<PlayerObjectInfo> players_info;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override ;
};
