#include <csignal>

#include "../lib/room_info.h"
#include "../lib/tcp_connection.h"
#include "io_context.h"

enum RoomStates {
  WAITING,
  STARTING,
  PLAYING
};

class Room {
public:
  explicit Room(const int server_fd, const int creator_fd);

  ~Room();

  void Run();
  void ManageEvents(int event_num, epoll_event* events);

  Room(const Room&) = delete;
  Room(Room&&) = delete;

private:
  IOContext io_context_;
  const int server_fd_;
  const int creator_fd_;
  std::unordered_map<int, TcpConnection<RoomStates> > connections_;
  static volatile sig_atomic_t exit_flag_;
  static void ExitHandler(int signum);
  void ConfigureSignals();
  void CloseConnection(const int socket_fd);
  //std::vector<Player> players_;
};