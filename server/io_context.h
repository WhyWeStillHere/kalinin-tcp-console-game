#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

enum EventType {
  SOCKET,
  READ_FD,
  WRITE_FD
};

const size_t MAX_EVENT_NUMBER = 4096;

class IOContext {
public:
  IOContext();
  ~IOContext();
  void AddEvent(const EventType event_type, const int fd_to_register);
  size_t Wait(epoll_event* &events);

private:
  void MakeNotBlocking(const int fd);

  int epoll_fd_ = -1;
  epoll_event events_[MAX_EVENT_NUMBER];

  struct data_t {
    int fd;
    EventType type;
  };
};