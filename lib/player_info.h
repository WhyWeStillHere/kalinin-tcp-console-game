#include <string>
#include <vector>

struct PlayerInfo {
  std::string login;

  void Read(int socket_fd);
  void WriteBuffer(std::vector<char>& buffer);
};