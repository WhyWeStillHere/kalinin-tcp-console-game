#include <cstdio>
#include <unistd.h>

class ScreenBuffer {
public:
  explicit ScreenBuffer(int rows, int cols);
  ~ScreenBuffer();

  char* operator[](int row);
  char* Data();
  std::size_t Size();

private:
  const std::size_t rows_, cols_;
  char* screen_buffer_;
};