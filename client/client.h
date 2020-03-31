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


class Client {
public:
  Client();
  ~Client();

  Client(const Client&) = delete;
  Client(Client&&) = delete;

  void Init();
  void Connect(const char* ip_string, const int port);
  void Run();

private:
  int socket_ = -1;
  static volatile sig_atomic_t exit_flag_;
  void ConfigureSignals();
  static void ExitHandler(int signum);
};