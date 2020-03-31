#pragma once

#include <vector>
#include <string>

struct RoomInfo {
  uint32_t id;
  uint32_t participant_num;

  int input_fd;
  std::vector<int> waiting_clients;
  pthread_t thread;

  void Read(int socket_fd);
  void WriteBuffer(std::vector<char>& buffer);
};

struct FullRoomInfo : public RoomInfo {
  std::vector<std::string> user_login;

  void Read(int socket_fd);
  void WriteBuffer(int socket_fd, std::vector<char>& buffer);
};