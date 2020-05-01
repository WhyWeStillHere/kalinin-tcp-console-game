#include "painter.h"
#include <cctype>
#include <stdexcept>

Painter::Painter(TerminalManager& terminal, const int rows, const int cols)
  : terminal_(terminal), screen_buffer_(rows, cols), rows_(rows), cols_(cols) {
};

void Painter::ApplyDrawing() {
  terminal_.Draw(screen_buffer_.Data(), screen_buffer_.Size());
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
  Set(x, y, ' ');
}

void Painter::DrawString(int x, int y, const char *str, std::size_t str_size) {
  for (int i = x, k = 0; k < str_size; ++i, ++k) {
    Set(i, y, str[k]);
  }
}

void Painter::DrawString(double x_rel, double y_rel, const char *str, std::size_t str_size) {
  DrawString(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel),
      str, str_size);
}

void Painter::DrawStringCenter(int x, int y, const char *str, std::size_t str_size) {
  int offset = str_size / 2;
  DrawString(x - offset, y, str, str_size);
}

void Painter::RemoveString(int x, int y, std::size_t str_size) {
  for (int i = x, k = 0; k < str_size; ++i, ++k) {
    Remove(i, y);
  }
}

void Painter::RemoveString(double x_rel, double y_rel, std::size_t str_size) {
  RemoveString(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel), str_size);
}

void Painter::RemoveStringCenter(int x, int y, std::size_t str_size) {
  int offset = str_size / 2;
  RemoveString(x - offset, y, str_size);
}

void Painter::RemoveStringCenter(double x_rel, double y_rel, std::size_t str_size) {
  RemoveStringCenter(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel), str_size);
}

void Painter::DrawStringCenter(double x_rel, double y_rel, const char *str, std::size_t str_size) {
  DrawStringCenter(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel), str, str_size);
}

int Painter::GetHorizontalCoord(double x_rel) {
  return x_rel * cols_;
}

int Painter::GetVerticalCoord(double y_rel) {
  return y_rel * rows_;
}

void Painter::ClearScreen() {
  for (int x = 0; x < cols_; ++x) {
    for (int y = 0; y < rows_; ++y) {
      Remove(x, y);
    }
  }
}

void Painter::MoveCursor(const int x, const int y) {
  cursor_.x = x;
  cursor_.y = y;
  terminal_.SetCursor(y + 1, x + 1);
}

void Painter::MoveCursor(const double x_rel, const double y_rel) {
  MoveCursor(GetHorizontalCoord(x_rel), GetVerticalCoord(y_rel));
}

void Painter::ShowCursor() {
  cursor_.is_visible = true;
  terminal_.ShowCursor();
}

void Painter::HideCursor() {
  cursor_.is_visible = false;
  terminal_.HideCursor();
}

void Painter::MoveCursor(Cursor::Direction direction) {
  switch (direction) {
    case Cursor::Direction::UP:
      if (cursor_.y >= 1) {
        --cursor_.y;
      }
      terminal_.MoveCursorUp();
      break;
    case Cursor::Direction::DOWN:
      if (cursor_.y + 1 <= rows_) {
        ++cursor_.y;
      }
      terminal_.MoveCursorDown();
      break;
  case Cursor::Direction::LEFT:
    if (cursor_.x >= 1) {
      --cursor_.x;
    }
    terminal_.MoveCursorLeft();
    break;
  case Cursor::Direction::RIGHT:
    if (cursor_.x + 1 <= cols_) {
      ++cursor_.x;
    }
    terminal_.MoveCursorRight();
    break;
  }
}

void Painter::Set(const char c) {
  Set(cursor_.x, cursor_.y, c);
}

char Painter::Get() {
  return Get(cursor_.x, cursor_.y);
}

void Painter::Remove() {
  Remove(cursor_.x, cursor_.y);
}

Cursor Painter::GetCursor() {
  return cursor_;
}