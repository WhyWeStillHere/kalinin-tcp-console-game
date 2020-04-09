#include "tcp_utilities.h"


void serialize_uint32(std::vector<char>& buffer, const uint32_t value) {
  net_unit32_t net_value;
  net_value.num = htonl(value);
  buffer.push_back(net_value.bytes[0]);
  buffer.push_back(net_value.bytes[1]);
  buffer.push_back(net_value.bytes[2]);
  buffer.push_back(net_value.bytes[3]);
}


char* buff_to_uint32(char* buffer, uint32_t* value) {
  net_unit32_t net_value;
  net_value.bytes[0] = buffer[0];
  net_value.bytes[1] = buffer[1];
  net_value.bytes[2] = buffer[2];
  net_value.bytes[3] = buffer[3];
  *value = ntohl(net_value.num);
  return buffer + 4;
}

void read_uint32(int socket_fd, uint32_t* value) {
  uint32_t net_val;
  ssize_t ret = read(socket_fd, &net_val, sizeof(net_val));
  if (ret == -1) {
    throw std::runtime_error("TCP block read error");
  }
  if (ret == 0) {
    throw std::runtime_error("Reached EOF");
  }
  *value = ntohl(net_val);
}

void read_uint32_by_char(const int socket_fd, uint32_t* value) {
  uint32_t net_val;
  char buffer[4];
  read_buffer(socket_fd, buffer, sizeof(buffer));
  buff_to_uint32(buffer, value);
}

void write_uint32(int socket_fd, uint32_t* value) {
  uint32_t net_val = htonl(*value);
  ssize_t ret = write(socket_fd, &net_val, sizeof(net_val));
  if (ret == -1) {
    throw std::runtime_error("TCP block write error");
  }
  if (ret == 0) {
    throw std::runtime_error("Reached EOF");
  }
}


void read_buffer(const int fd, char* buffer, const size_t buffer_size) {
  size_t read_size = buffer_size;
  size_t read_cnt = 0;
  while (read_cnt != read_size) {
    ssize_t ret = read(fd, buffer + read_cnt,
                       read_size - read_cnt);
    if (ret == -1) {
      throw std::runtime_error("TCP block read error");
    }
    if (ret == 0) {
      throw std::runtime_error("Reached EOF");
    }
    read_cnt += ret;
  }
}

void write_buffer(const int fd, const char* buffer, const size_t buffer_size) {
  size_t write_size = buffer_size;
  size_t write_cnt = 0;
  while (write_cnt != write_size) {
    ssize_t ret = write(fd, buffer + write_cnt,
                        write_size - write_cnt);
    if (ret == -1) {
      throw std::runtime_error("TCP block write error");
    }
    if (ret == 0) {
      throw std::runtime_error("Reached EOF");
    }
    write_cnt += ret;
  }
}

size_t write_buffer_some(const int fd, const char* buffer, const size_t buffer_size) {
  size_t write_size = buffer_size;
  size_t write_cnt = 0;
  while (write_cnt != write_size) {
    ssize_t ret = write(fd, buffer + write_cnt,
                        write_size - write_cnt);
    if (ret == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      throw std::runtime_error("TCP nonblock write error");
    }
    if (ret == 0) {
      throw std::runtime_error("Reached EOF");
    }
    write_cnt += ret;
  }
  return write_cnt;
}

size_t read_buffer_some(const int fd, char* buffer, const size_t buffer_size) {
  size_t read_size = buffer_size;
  size_t read_cnt = 0;
  while (read_cnt != read_size) {
    ssize_t ret = read(fd, buffer + read_cnt,
                       read_size - read_cnt);
    if (ret == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      throw std::runtime_error("TCP read error");
    }
    if (ret == 0) {
      throw std::runtime_error("Reached EOF");
    }
    read_cnt += ret;
  }
  return read_cnt;
}

void write_string(const int fd, const std::string& str) {
  uint32_t str_size = str.size();
  write_uint32(fd, &str_size);
  write_buffer(fd, str.data(), str_size);
}

void serialize_string(std::vector<char>& buffer, const std::string& str) {
  uint32_t str_size = str.size();
  serialize_uint32(buffer, str_size);
  for (const auto& letter: str) {
    buffer.push_back(letter);
  }
}

std::string read_string(const int fd) {
  uint32_t str_size;
  read_uint32(fd, &str_size);
  std::string str(str_size, ' ');
  read_buffer(fd, &str[0], str_size);
}