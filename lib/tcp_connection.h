#pragma once

#include <vector>

template <class ConnectionState>
struct TcpConnection {
  std::vector<char> buffer;
  size_t operation_index = 0;
  ConnectionState state;
  ConnectionState key_state;
};