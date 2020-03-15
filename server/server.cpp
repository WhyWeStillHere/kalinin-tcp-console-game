
#include "server.h"

volatile sig_atomic_t GameServer::exit_flag_ = 0;

GameServer::GameServer(const ServerStartUpType type) {
  if (type == DAEMON) {
    GameServer::ConfigureDaemon();
  }
  ConfigureSignals();
}

GameServer::~GameServer() {
  if (socket_ != -1) {
    shutdown(socket_, SHUT_RDWR);
    char buffer[4096];
    int ret;
    do {
      ret = read(socket_, buffer, sizeof(buffer));
    } while (ret > 0);
    close(socket_);
    syslog (LOG_NOTICE, "Socket correctly closed");
  }
  syslog (LOG_NOTICE, "Server terminated");
  closelog();
}

void GameServer::ExitHandler(int signum) {
  exit_flag_ = 1;
}

void GameServer::ConfigureSignals() {
  struct sigaction action_int;
  memset(&action_int, 0, sizeof(action_int));
  action_int.sa_handler = GameServer::ExitHandler;
  int ret;
  ret = sigaction(SIGTERM, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }
  ret = sigaction(SIGINT, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }
}

void GameServer::ConfigureDaemon()
{
  pid_t pid;
  pid = fork();

  if (pid < 0) {
    perror("Unable to make fork\n");
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  // Get new Id in system
  if (setsid() < 0) {
    perror("setsid() function failure\n");
    exit(EXIT_FAILURE);
  }

  // Catch, ignore and handle signals
  // Block all signals except SIGTERM and SIGINT
  sigset_t sig_mask;
  sigfillset(&sig_mask);
  sigdelset(&sig_mask, SIGTERM);
  sigdelset(&sig_mask, SIGINT);
  sigprocmask(SIG_SETMASK, &sig_mask, NULL);


  // Set new file permissions rw-rw-rw-
  umask(0666);

  // Change the working directory to the root directory
  chdir("/");

  // Close all open file descriptors
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // Open the log file
  openlog ("Server daemon", LOG_PID, LOG_DAEMON);
  syslog (LOG_NOTICE, "Server daemon started");
}

void GameServer::Init(const char* ip_string, const int port) {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    syslog(LOG_NOTICE, "Can not create socket: %s", strerror(errno));
    throw std::runtime_error("Unable to create socket");
  }
  syslog (LOG_NOTICE, "Socket created");

  // Make ip adress from string
  struct in_addr server_in_addr;
  inet_aton(ip_string, &server_in_addr);

  struct sockaddr_in server_sockaddr;
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_addr = server_in_addr;
  server_sockaddr.sin_port = htons(port);

  // Naming of socket
  int ret = bind(socket_, (const struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr_in));
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while bind(): %s", strerror(errno));
    throw std::runtime_error("Binding error");
  }
  syslog (LOG_NOTICE, "Binding complete");

  ret = listen(socket_, SOMAXCONN); // Set max length of queue(128)
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while listen(): %s", strerror(errno));
    throw std::runtime_error("Unable to manage listening from socket");
  }

  syslog(LOG_NOTICE, "Server initialized for: %s %d", ip_string, port);
}
void GameServer::Run() {
  IOContext io_context;
  io_context.AddEvent(SOCKET, socket_);
  epoll_event* events;
  while (!exit_flag_) {
    try {
      int event_num = io_context.Wait(events);
    } catch (...) {
      syslog (LOG_WARNING, "Error in server runtime");
    }
  }
  syslog (LOG_NOTICE, "Server stopped");
}


