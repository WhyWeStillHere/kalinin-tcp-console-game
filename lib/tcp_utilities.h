#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <vector>

// Structure to ease splitting uint32_t as 4 chars
union net_unit32_t{
  uint32_t num;
  char bytes[4];
};

// Write uint32_t as 4 chars
void serialize_uint32(std::vector<char>& buffer, const uint32_t value);
// Get uint32 from chars
char* buff_to_uint32(char* buffer, uint32_t* value);

void read_uint32(const int socket_fd, uint32_t* value);
void read_uint32_by_char(const int socket_fd, uint32_t* value);
// Read data in buffer
void read_buffer(const int fd, char* buffer, const size_t buffer_size);

void write_uint32(int socket_fd, uint32_t* value);
void write_buffer(const int fd, const char* buffer, const size_t buffer_size);

// Write some data to buffer
size_t write_buffer_some(const int fd, const char* buffer, const size_t buffer_size);
// Read some data to buffer
size_t read_buffer_some(const int fd, char* buffer, const size_t buffer_size);

// First 4 char would be str len
void write_string(const int fd, const std::string& str);
void serialize_string(std::vector<char>& buffer, const std::string& str);
// First 4 char would be str len
std::string read_string(const int fd);