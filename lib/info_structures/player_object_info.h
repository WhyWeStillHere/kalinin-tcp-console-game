#include "info_structure.h"

class PlayerObjectInfo: public InfoStructure {
public:
  uint32_t id;
  uint32_t health_value;

  void Read(int socket_fd) override ;
  void WriteBuffer(std::vector<char>& buffer) const override ;
};
