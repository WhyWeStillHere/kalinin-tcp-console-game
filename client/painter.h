#include "terminal_manager.h"
#include "screen_buffer.h"
#include <vector>

struct Cursor {
  int x = 0, y = 0;
  bool is_visible = false;

  enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
  };
};

class Painter {
public:
  explicit Painter(TerminalManager& terminal, int rows, int cols);

  void ApplyDrawing();

  void DrawVerticalLine(int x, int y1, int y2, char sym);
  void DrawHorizontalLine(int y, int x1, int x2, char sym);
  void DrawHorizontalLine(double y_rel, double x1_rel, double x2_rel, char sym);
  void DrawVerticalLine(double x_rel, double y1_rel, double y2_rel, char sym);
  void DrawRectangle(int x1, int y1, int x2, int y2,
      char vert_sym = '|', char hor_sym = '-');
  void DrawRectangle(double x1_rel, double y1_rel, double x2_rel, double y2_rel,
                     char vert_sym = '|', char hor_sym = '-');
  void DrawString(int x, int y, const char* str, std::size_t str_size);
  void DrawString(double x_rel, double y_rel, const char* str, std::size_t str_size);
  void DrawStringCenter(int x, int y, const char* str, std::size_t str_size);
  void DrawStringCenter(double x_rel, double y_rel, const char* str, std::size_t str_size);
  int GetHorizontalCoord(double x_rel);
  int GetVerticalCoord(double y_rel);
  void DrawFrame(char vert_sym = '|', char hor_sym = '-');
  void Set(int x, int y, char c);
  char Get(int x, int y);
  void Remove(int x, int y);
  void RemoveString(int x, int y, std::size_t str_size);
  void RemoveString(double x_rel, double y_rel, std::size_t str_size);
  void RemoveStringCenter(int x, int y, std::size_t str_size);
  void RemoveStringCenter(double x_rel, double y_rel, std::size_t str_size);

  // Using coords of cursor
  void Set(char c);
  char Get();
  void Remove();

  void ClearScreen();

  void MoveCursor(int x, int y);
  void MoveCursor(double x_rel, double y_rel);
  void MoveCursor(Cursor::Direction direction);
  void ShowCursor();
  void HideCursor();
  Cursor GetCursor();

private:
  const size_t rows_, cols_;
  Cursor cursor_;
  TerminalManager& terminal_;
  ScreenBuffer screen_buffer_;
};