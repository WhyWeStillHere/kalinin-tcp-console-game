#include "room.h"
#include <cstring>
#include <sys/socket.h>
#include <sys/syslog.h>

volatile sig_atomic_t Room::exit_flag_ = 0;

Room::Room(const int server_fd, const int creator_fd)
    : server_fd_(server_fd), creator_fd_(creator_fd) {
  ConfigureSignals();
}

void Room::ExitHandler(int signum) {
  exit_flag_ = 1;
}

Room::~Room() {
  while (!connections_.empty()) {
    CloseConnection((*connections_.begin()).first);
  }
  CloseConnection(creator_fd_);
  if (server_fd_ != -1) {
    char buffer[4096];
    int ret;
    do {
      ret = read(server_fd_, buffer, sizeof(buffer));
    } while (ret > 0);
    close(server_fd_);
    syslog (LOG_NOTICE, "Server fd correctly closed in room");
  }
  syslog (LOG_NOTICE, "Room terminated");
}

void Room::CloseConnection(const int socket_fd) {
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

void Room::ConfigureSignals() {
  struct sigaction action_int;
  memset(&action_int, 0, sizeof(action_int));
  action_int.sa_handler = Room::ExitHandler;
  int ret;
  /*ret = sigaction(SIGTERM, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }*/
  ret = sigaction(SIGINT, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }
}

void Room::Run() {
  io_context_.AddEvent(READ_FD, SERVER, server_fd_);
  io_context_.AddEvent(READ_FD, CREATOR, creator_fd_);
  epoll_event* events;
  while (!exit_flag_) {
    try {
      int event_num = io_context_.Wait(events);
      //syslog (LOG_WARNING, "Room events %d", event_num);
      //ManageEvents(event_num, events);
    } catch (...) {
      syslog (LOG_WARNING, "Error in room runtime");
    }
  }
  syslog (LOG_NOTICE, "Room stopped");
}

void Room::ManageEvents(const int event_num, epoll_event* events) {
  for (size_t i = 0; i < event_num; ++i) {
    epoll_event* event = &events[i];
    auto data = static_cast<IOContext::data_t *>(event->data.ptr);


    if (event->events & (EPOLLHUP | EPOLLERR)) {
      if (data->fd_type == CREATOR) {
        syslog (LOG_WARNING, "Host error");
        exit_flag_ = 1;
        return;
      }
      CloseConnection(data->fd);
      continue;
    }

    if (data->fd_type == CLIENT) {
      syslog (LOG_WARNING, "TODO shit");
    }
    if (data->fd_type == CREATOR) {
      try {
        char buffer[4];
        int ret = read(data->fd, buffer, sizeof(buffer));
        //syslog (LOG_WARNING, "Creator activity");
      } catch(...) {
        syslog (LOG_WARNING, "Error while room connection");
        std::rethrow_exception(std::current_exception());
      }
    }
  }
}