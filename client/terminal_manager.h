#pragma once

#ifndef CONSOLE_GAME_TERMINAL_MANAGER_H
#define CONSOLE_GAME_TERMINAL_MANAGER_H

#include <cstdio>
#include <unistd.h>
#include <termios.h>


class TerminalManager {
public:
  ~TerminalManager();
  TerminalManager() = default;

  void Init();
  void Draw(char* buffer, std::size_t buffer_size);
  void ClearScreen();
  int GetWindowSize(int& rows, int& cols);

  int ReadKey();
  char ReadKeyBlock();
  void ClearInput();

  void HideCursor();
  void ShowCursor();
  // Right-left corner by default
  void SetCursor(int row = 0, int col = 0); // Absolute position
  void MoveCursorUp(int steps = 1);
  void MoveCursorDown(int steps = 1);
  void MoveCursorLeft(int steps = 1);
  void MoveCursorRight(int steps = 1);
  void SaveCursor();
  void RestoreCursor();

private:

  int GetCursorPosition(int& rows, int& cols);
  int GetWindowSize();
  void EnableRawMode();
  void DisableRawMode();

  void Die(const char* s);
  bool Write(const char* s, int str_len, bool die = false);

  int terminal_rows_;
  int terminal_cols_;
  termios orig_termios_;
};

#endif // CONSOLE_GAME_TERMINAL_MANAGER_H
