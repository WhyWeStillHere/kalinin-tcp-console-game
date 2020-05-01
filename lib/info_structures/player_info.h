#include <string>
#include <vector>

#include "info_structure.h"

struct PlayerInfo : public InfoStructure {
  uint32_t id;
  std::string login;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override ;
};
