#pragma once

#include <unordered_map>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <unordered_map>

enum EventType {
  READ_FD,
  WRITE_FD
};

enum FdType {
  SERVER,
  SOCKET,
  CLIENT,
  ROOM,
  CREATOR,
  TIMER
};

const size_t MAX_EVENT_NUMBER = 4096;

class IOContext {
public:
  struct data_t {
    EventType action_type;
    FdType fd_type;
    int fd;
  };

  IOContext();
  ~IOContext();
  void AddEvent(const EventType event_type, const FdType fd_type, const int fd);
  void ChangeEvent(const EventType new_type, const int fd);
  void DelEvent(const int fd);
  void SetTimer(int milliseconds);
  size_t Wait(epoll_event* &events);

private:
  void MakeNotBlocking(const int fd);
  void SetEventType(const EventType type, epoll_event& event);

  int epoll_fd_ = -1;
  epoll_event events_[MAX_EVENT_NUMBER];
  std::unordered_map<int, data_t*> event_map_;
};