#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/syslog.h>

#include "io_context.h"

IOContext::IOContext() {
  epoll_fd_ = epoll_create1(0);
  if (epoll_fd_ < 0) {
    syslog(LOG_NOTICE, "Error while creating: %s", strerror(errno));
    throw std::runtime_error("Can not create epoll in context");
  }
}

IOContext::~IOContext() {
  if (epoll_fd_ != -1) {
    close(epoll_fd_);
  }
  for (auto event_data: event_map_) {
    free(event_data.second);
  }
}

void IOContext::MakeNotBlocking(const int fd) {
  int flags = fcntl(fd, F_GETFL);
  if (flags < 0) {
    syslog(LOG_NOTICE, "Error in fcntl: %s", strerror(errno));
    throw std::runtime_error("Fcntl error");
  }
  if (flags & O_NONBLOCK) {
    return;
  }
  flags |= O_NONBLOCK;
  int ret = fcntl(fd, F_SETFL, flags);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error in fcntl: %s", strerror(errno));
    throw std::runtime_error("Fcntl error");
  }
}

void IOContext::SetEventType(const EventType type, epoll_event &event) {
  switch (type) {
  case READ_FD:
    event.events = EPOLLIN; // 1
    break;
  case WRITE_FD:
    event.events = EPOLLOUT; // 4
    break;
  }
}

void IOContext::AddEvent(const EventType event_type,
    const FdType fd_type, const int fd) {
  MakeNotBlocking(fd);

  epoll_event event;
  SetEventType(event_type, event);

  data_t* data = static_cast<data_t*>(malloc(sizeof(struct data_t)));
  data->action_type = event_type;
  data->fd_type = fd_type;
  data->fd = fd;
  event.data.ptr = data;

  event_map_[fd] = data;

  int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while adding event: %s", strerror(errno));
    throw std::runtime_error("Unable to add event");
  }
}

void IOContext::DelEvent(const int fd) {
  if (event_map_.find(fd) == event_map_.end()) {
    throw std::logic_error("No such event in io_context");
  }

  free(event_map_[fd]);
  event_map_.erase(fd);

  int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while event deletion: %s", strerror(errno));
    throw std::runtime_error("Unable to delete event");
  }
}

void IOContext::ChangeEvent(const EventType new_type, const int fd) {
  if (event_map_.find(fd) == event_map_.end()) {
    throw std::logic_error("No such event in io_context");
  }

  epoll_event event;
  SetEventType(new_type, event);

  event_map_[fd]->action_type = new_type;
  event.data.ptr = event_map_[fd];


  int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while changing event: %s", strerror(errno));
    throw std::runtime_error("Unable to change event");
  }
}

size_t IOContext::Wait(epoll_event* &events) {
  int event_num = epoll_wait(
      epoll_fd_,
      events_,
      MAX_EVENT_NUMBER,
      -1 // Timeout
  );
  if (event_num == -1) {
    event_num = 0;
  }
  events = events_;
  return event_num;
}
