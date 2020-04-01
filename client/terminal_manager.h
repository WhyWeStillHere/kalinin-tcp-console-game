#pragma once

#ifndef CONSOLE_GAME_TERMINAL_MANAGER_H
#define CONSOLE_GAME_TERMINAL_MANAGER_H

#include <cstdio>
#include <unistd.h>
#include <termios.h>

#define CTRL_KEY(k) ((k) & 0x1f) // K & 00011111

class TerminalManager {
public:
  ~TerminalManager();
  TerminalManager() = default;

  void Init();
  void Draw(char* buffer, std::size_t buffer_size);
  void ClearScreen();
  int GetWindowSize(int& rows, int& cols);

  char ReadKey();
  void ClearInput();

  void HideCursor();
  void ShowCursor();

private:
  void EmptyScreen();

  // Right-left corner by default
  void MoveCursor(int line = 0, int column = 0); // Absolute position

  int GetCursorPosition(int& rows, int& cols);
  int GetWindowSize();
  void EnableRawMode();
  void DisableRawMode();
  void Die(const char* s);

  int terminal_rows_;
  int terminal_cols_;
  termios orig_termios_;
};

#endif // CONSOLE_GAME_TERMINAL_MANAGER_H
