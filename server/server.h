#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "io_context.h"

enum ServerStartUpType {
  DAEMON,
  PROGRAM
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
  int socket_ = -1;
  static volatile sig_atomic_t exit_flag_;
  void ConfigureDaemon();
  static void ExitHandler(int signum);
  void ConfigureSignals();
};