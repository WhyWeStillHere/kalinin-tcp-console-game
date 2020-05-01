#include "screen_buffer.h"
#include <cstdlib>
#include <stdexcept>

ScreenBuffer::ScreenBuffer(int rows, int cols)
  : rows_(rows), cols_(cols)
  {
  if (rows_ <= 0 || cols_ <= 0) {
    throw std::logic_error("Incorrect buffer size");
  }
  screen_buffer_ = static_cast<char*>(malloc(rows_ * (cols_ + 2)));
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      screen_buffer_[i * (cols_ + 2) + j] = ' ';
    }
    screen_buffer_[i * (cols_ + 2) + cols_] = '\r';
    screen_buffer_[i * (cols_ + 2) + cols + 1] = '\n';
  }
}

ScreenBuffer::~ScreenBuffer() {
  free(screen_buffer_);
}

char* ScreenBuffer::operator[](const int row) {
  return &screen_buffer_[(cols_ + 2) * row];
}

char* ScreenBuffer::Data() {
  return screen_buffer_;
}

std::size_t ScreenBuffer::Size() {
  return (cols_ + 2) * rows_ - 2;
}