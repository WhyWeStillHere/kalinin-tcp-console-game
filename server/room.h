#include <csignal>

#include "../lib/info_structures/player_info.h"
#include "../lib/info_structures/room_info.h"
#include "../lib/network_utilities/tcp_connection.h"
#include "io_context.h"

enum PlayerStates{
  ASKING_LOGIN_SIZE,
  ASKING_LOGIN,
  WRITE_PLAYERS_NUM,
  WRITE_PLAYERS,
  READY_TO_PLAY,
  STARTING_TO_PLAY
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
  void ManagePlayer(int fd);
  void AddPlayer(int fd);
  void UpdatePlayerList();
  void RemovePlayer(int fd);
  void ManageHost(int fd);

  IOContext io_context_;
  const int server_fd_;
  const int creator_fd_;
  RoomStates state_;
  std::unordered_map<int, TcpConnection<PlayerStates> > connections_;
  static volatile sig_atomic_t exit_flag_;
  static void ExitHandler(int signum);
  void ConfigureSignals();
  void CloseConnection(const int socket_fd);
  std::unordered_map<int, PlayerInfo> players_;
  PlayerInfo host_;
  int waiting_persons_ = 0;
};