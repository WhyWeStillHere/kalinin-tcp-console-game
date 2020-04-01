#include "painter.h"
#include <cctype>
#include <stdexcept>

Painter::Painter(TerminalManager& terminal, const int rows, const int cols)
  : terminal_(terminal), screen_buffer_(rows, cols), rows_(rows), cols_(cols) {
};

void Painter::ApplyDrawing() {
  terminal_.Draw(screen_buffer_.Data(), screen_buffer_.Size());
  terminal_.HideCursor();
}

void Painter::Set(const int x, const int y, const char c) {
  if (x < 0 || x >= cols_ || y < 0 || y >= rows_) {
    throw std::logic_error("Incorrect screen buffer point");
  }
  screen_buffer_[y][x] = c;
}

char Painter::Get(const int x, const int y) {
  if (x < 0 || x >= cols_ || y < 0 || y >= rows_) {
    throw std::logic_error("Incorrect screen buffer point");
  }
  return screen_buffer_[y][x];
}

void Painter::DrawVerticalLine(const int x, const int y1, const int y2,
                               const char sym) {
  if (iscntrl(sym)) {
    throw std::logic_error("Incorrect arguments");
  }
  int start = y1;
  int end = y2;
  if (end < start) {
    std::swap(start, end);
  }
  for (int i = start; i <= end; ++i) {
    Set(x, i, sym);
  }
}

void Painter::DrawHorizontalLine(const int y, const int x1, const int x2,
                                 const char sym) {
  if (iscntrl(sym)) {
    throw std::logic_error("Incorrect arguments");
  }
  int start = x1;
  int end = x2;
  if (end < start) {
    std::swap(start, end);
  }
  for (int i = start; i <= end; ++i) {
    Set(i, y, sym);
  }
}

void Painter::DrawVerticalLine(const double x_rel, const double y1_rel,
                               const double y2_rel, const char sym) {

  DrawVerticalLine(GetHorizontalCoord(x_rel), GetVerticalCoord(y1_rel),
                   GetVerticalCoord(y2_rel), sym);
}

void Painter::DrawHorizontalLine(const double y_rel, const double x1_rel,
                                 const double x2_rel, char sym) {
  DrawHorizontalLine(GetVerticalCoord(y_rel), GetHorizontalCoord(x1_rel),
                     GetHorizontalCoord(x2_rel), sym);
}

void Painter::DrawRectangle(const int x1,const  int y1, const  int x2,  const int y2,
                            const char vert_sym, const char hor_sym) {
  DrawHorizontalLine(y1, x1, x2, hor_sym);
  DrawHorizontalLine(y2, x1, x2, hor_sym);
  DrawVerticalLine(x1, y1, y2, vert_sym);
  DrawVerticalLine(x2, y1, y2, vert_sym);
}


void Painter::DrawFrame(const char vert_sym, const char hor_sym) {
  DrawRectangle(0, 0, cols_ - 1, rows_ - 1);
}

void Painter::DrawRectangle(const double x1_rel, const double y1_rel,
    const double x2_rel, const double y2_rel, char vert_sym, char hor_sym) {
  DrawRectangle(GetHorizontalCoord(x1_rel), GetVerticalCoord(y1_rel),
                GetHorizontalCoord(x2_rel), GetVerticalCoord(y2_rel),
                vert_sym, hor_sym);
}

void Painter::Remove(int x, int y) {
  if (x < 0 || x >= cols_ || y < 0 || y >= rows_) {
    throw std::logic_error("Incorrect screen buffer point");
  }
  Set(x, y, ' ');
}

void Painter::DrawString(int x, int y, char *str, std::size_t str_size) {
  for (int i = x, k = 0; k < str_size; ++i, ++k) {
    Set(i, y, str[k]);
  }
}

void Painter::DrawString(double x_rel, double y_rel, char *str, std::size_t str_size) {
  DrawString(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel),
      str, str_size);
}

void Painter::DrawStringCenter(int x, int y, char *str, std::size_t str_size) {
  int offset = str_size / 2;
  DrawString(x - offset, y, str, str_size);
}

void Painter::DrawStringCenter(double x_rel, double y_rel, char *str, std::size_t str_size) {
  DrawStringCenter(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel), str, str_size);
}

int Painter::GetHorizontalCoord(double x_rel) {
  return x_rel * cols_;
}

int Painter::GetVerticalCoord(double y_rel) {
  return y_rel * rows_;
}
