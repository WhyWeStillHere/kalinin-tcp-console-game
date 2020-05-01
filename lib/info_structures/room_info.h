#pragma once

#include "info_structure.h"
#include <string>
#include <vector>

enum RoomStates {
  WAITING,
  STARTING,
  PLAYING
};

struct RoomInfo : public InfoStructure {
  uint32_t id;
  uint32_t participant_num = 0;

  int input_fd;
  std::vector<int> waiting_clients;
  pthread_t thread;
  RoomStates state = WAITING;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override;
};
