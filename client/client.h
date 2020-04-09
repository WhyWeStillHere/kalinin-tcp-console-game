#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "interface_manager.h"

class Client {
public:
  Client();
  ~Client();

  Client(const Client&) = delete;
  Client(Client&&) = delete;

  void Init();
  void Connect(const char* ip_string, const int port);
  void Run(const char* ip_string, const int port);

private:
  int socket_ = -1;
  static volatile sig_atomic_t exit_flag_;
  std::string last_error_ = "";
  void ConfigureSignals();
  static void ExitHandler(int signum);
  InterfaceManager interface_manager_;
};