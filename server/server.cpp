
#include "server.h"
#include "../lib/tcp_utilities.h"
#include <iostream>

volatile sig_atomic_t GameServer::exit_flag_ = 0;

GameServer::GameServer(const ServerStartUpType type) {
  if (type == DAEMON) {
    GameServer::ConfigureDaemon();
  }
  ConfigureSignals();
}

GameServer::~GameServer() {
  syslog (LOG_NOTICE, "Start destruction");
  while (!connections_.empty()) {
    CloseConnection((*connections_.begin()).first);
  }

  for (auto& room: rooms_) {
    int ret = fcntl(room.second.input_fd, F_GETFD);
    if (ret != -1) {
      pthread_kill(room.second.thread, SIGINT);
    } else {
      room.second.input_fd = -1;
    }
  }

  for (auto& room: rooms_) {
    if (room.second.input_fd != -1) {
      pthread_join(room.second.thread, NULL);
    }
  }

  syslog (LOG_NOTICE, "Rooms destroyed");

  if (socket_ != -1) {
    char buffer[4096];
    int ret;
    do {
      ret = read(socket_, buffer, sizeof(buffer));
    } while (ret > 0);
    close(socket_);
    syslog (LOG_NOTICE, "Server socket correctly closed");
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
  io_context_.AddEvent(READ_FD, SOCKET, socket_);
  epoll_event* events;
  while (!exit_flag_) {
    try {
      int event_num = io_context_.Wait(events);
      syslog (LOG_WARNING, "Events %d", event_num);
      ManageEvents(event_num, events);
    } catch (...) {
      syslog (LOG_WARNING, "Error in server runtime");
    }
  }
  syslog (LOG_NOTICE, "Server stopped");
}

void GameServer::ManageEvents(const int event_num, epoll_event* events) {
  for (size_t i = 0; i < event_num; ++i) {
    epoll_event* event = &events[i];
    auto data = static_cast<IOContext::data_t *>(event->data.ptr);

    if (event->events & (EPOLLHUP | EPOLLERR | EPOLLPRI)) {
      CloseConnection(data->fd);
      continue;
    }

    if (data->fd_type == SOCKET) {
      AddNewConnection();
    }
    if (data->fd_type == CLIENT) {
      try {
        ManageState(data->fd);
      } catch(...) {
        CloseConnection(data->fd);
      }
    }
    if (data->fd_type == ROOM) {
      try {
        ConnectToRoom(data->fd);
      } catch(...) {
        syslog (LOG_WARNING, "Error while room connection");
        std::rethrow_exception(std::current_exception());
      }
    }
  }
}

void GameServer::ConnectToRoom(int fd) {
  RoomInfo room = rooms_.at(fd);
  while (!room.waiting_clients.empty()) {
    int ret = write(fd, &room.waiting_clients.back(), sizeof(int));
    if (ret <= 0) {
      return;
    }
    room.waiting_clients.pop_back();
  }
  io_context_.DelEvent(fd);
}

void GameServer::ManageState(int fd) {
  ManagerStates& state = connections_[fd].state;
  std::vector<char>& buffer = connections_[fd].buffer;
  size_t& index = connections_[fd].operation_index;

  if (state == START) {
    serialize_uint32(buffer, static_cast<uint32_t>(rooms_.size()));
    for (auto room_info: rooms_) {
      room_info.second.WriteBuffer(buffer);
    }
    state = SENDING_ROOMS;
  }
  if (state == SENDING_ROOMS) {
    char* buf = buffer.data();
    index += write_buffer_some(fd,
       buf + static_cast<int>(index),
       buffer.size() * sizeof(char));
    if (index == buffer.size()) {
     buffer.clear();
     state = WAITING_COMMAND;
     io_context_.ChangeEvent(READ_FD, fd);
     return;
    }
  }
  if (state == WAITING_COMMAND) {
    auto command = ReadCommand<CommandToManager>(fd);
    if (command == CommandToManager::REFRESH_ROOMS) {
      state = START;
      io_context_.ChangeEvent(WRITE_FD, fd);
    } else if (command == CommandToManager::CREATE_ROOM) {
      state = ASKING_LOGIN_SIZE;
      index = 0;
      return;
    } else if (command == CommandToManager::JOIN_ROOM) {
      state = JOINING_ROOM;
      return;
    } else {
      syslog (LOG_WARNING, "Error in receiving command");
      return;
    }
  }
  if (state == ASKING_LOGIN_SIZE) {
    uint32_t login_size;
    read_uint32(fd, &login_size);
    buffer.resize(login_size);
    state = ASKING_LOGIN;
  }
  if (state == ASKING_LOGIN) {
    index += read_buffer_some(fd, buffer.data() + index, buffer.size());
    if (index == buffer.size()) {
      syslog (LOG_WARNING, "New login: %s", buffer.data());
      buffer.clear();
      CreateRoom(fd);
    }
    return;
  }
  if (state == JOINING_ROOM) {
    uint32_t room_pid;
    read_uint32(fd, &room_pid);

    if (rooms_.find(fd) == rooms_.end()) {
      throw std::runtime_error("Pid is not valid");
    }
    RoomInfo room = rooms_.at(fd);
    room.waiting_clients.push_back(fd);
    io_context_.DelEvent(fd);
    io_context_.AddEvent(WRITE_FD, ROOM, room.input_fd);
  }
}

void GameServer::CreateRoom(int fd) {
  int room_fd[2];
  int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, room_fd);
  if (ret < 0) {
    throw std::runtime_error("Unable to create socketpair");
  }
  io_context_.DelEvent(fd);
  connections_.erase(connections_.find(fd));

  RoomInfo info;
  info.id = ID++;
  info.participant_num = 0;
  info.input_fd = room_fd[1];

  int args[2];
  args[0] = room_fd[0];
  args[1] = fd;

  pthread_create(&info.thread, NULL, RoomTread, (void*)args);
  rooms_[info.id] = info;
}

void* GameServer::RoomTread(void *arg) {
  // First arg - server fd
  // Second arg - creator fd
  auto fds = static_cast<int* >(arg);
  Room room(fds[0], fds[1]);
  room.Run();
  return NULL;
}

void GameServer::AddNewConnection() {
  int client_fd;
  do {
    client_fd = accept4(socket_, NULL, NULL, SOCK_NONBLOCK);
    if (client_fd == -1) {
      break;
    }
    connections_[client_fd].state = START;
    io_context_.AddEvent(WRITE_FD, CLIENT, client_fd);
    syslog (LOG_NOTICE, "Accept connection");
  } while (1);
}

void GameServer::CloseConnection(const int socket_fd) {
  io_context_.DelEvent(socket_fd);
  shutdown(socket_fd, SHUT_RDWR);
  connections_.erase(socket_fd);
  char buffer[4096];
  int ret;
  do {
    ret = read(socket_fd, buffer, sizeof(buffer));
  } while (ret > 0);
  close(socket_fd);
  syslog (LOG_NOTICE, "Connection closed");
}


