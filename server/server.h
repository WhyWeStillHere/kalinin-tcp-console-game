#pragma once

#include <arpa/inet.h>
#include <unordered_map>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <pthread.h>

#include "../lib/info_structures/room_info.h"
#include "../lib/network_utilities/send_command.h"
#include "../lib/network_utilities/tcp_connection.h"
#include "../lib/network_utilities/tcp_utilities.h"
#include "io_context.h"
#include "room.h"

enum ServerStartUpType {
  DAEMON,
  PROGRAM
};

enum ManagerStates {
  START,
  SENDING_ROOMS,
  WAITING_COMMAND,
  JOINING_ROOM,
  CREATING_ROOM
};

class GameServer {
public:
  //TODO: Bad place for configure!!!
  explicit GameServer(const ServerStartUpType type);

  ~GameServer();

  GameServer(const GameServer&) = delete;
  GameServer(GameServer&&) = delete;

  void Run();
  void Init(const char* ip_string, const int port);

private:
  void ConfigureDaemon();
  void CloseConnection(int socket_fd);
  static void ExitHandler(int signum);
  void ConfigureSignals();
  void ManageEvents(int event_num, epoll_event* events);
  void AddNewConnection();
  void ManageState(int fd);
  void ConnectToRoom(int fd);

  void CreateRoom(int fd);
  static void* RoomTread(void* arg);

  int socket_ = -1;
  uint64_t ID = 1;
  static volatile sig_atomic_t exit_flag_;
  std::unordered_map<int, TcpConnection<ManagerStates> > connections_;
  std::unordered_map<int, RoomInfo> rooms_;
  IOContext io_context_;
};