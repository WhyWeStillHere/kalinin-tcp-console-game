#include "io_context.h"
#include <cstring>
#include <stdexcept>
#include <sys/syslog.h>

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

void IOContext::AddEvent(const EventType event_type, const int fd_to_add) {
  MakeNotBlocking(fd_to_add);

  struct epoll_event event;
  switch (event_type) {
  case SOCKET:
    event.events = EPOLLIN;
    break;
  case READ_FD:
    event.events = EPOLLIN;
  case WRITE_FD:
    event.events = EPOLLOUT;
    break;
  }
  event.events = EPOLLIN;
  data_t* data = static_cast<data_t*>(malloc(sizeof(struct data_t)));
  data->fd = fd_to_add;
  data->type = event_type;
  event.data.ptr = data;

  int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd_to_add, &event);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while adding event: %s", strerror(errno));
    throw std::runtime_error("Unable to add event");
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
