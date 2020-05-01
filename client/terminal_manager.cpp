#include "terminal_manager.h"
#include <cerrno>
#include <cstring>
#include <exception>
#include <system_error>
#include <sys/ioctl.h>

void TerminalManager::Init() {
  if (GetWindowSize() == -1) {
    Die("GetWindowSize");
  }
  EnableRawMode();
}

TerminalManager::~TerminalManager() {
  ClearScreen();
  SetCursor();

  DisableRawMode();
}

void TerminalManager::Draw(char *buffer, std::size_t buffer_size) {
  SaveCursor();
  ClearScreen();
  SetCursor();
  Write(buffer, buffer_size, true);
  RestoreCursor();
}

void TerminalManager::SaveCursor() {
  Write("\x1b[s", 3, true);
}

void TerminalManager::RestoreCursor() {
  Write("\x1b[u", 3, true);
}

char TerminalManager::ReadKeyBlock() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) {
      Die("read");
    }
  }
  return c;
}

int TerminalManager::ReadKey() {
  int nread;
  char c;
  nread = read(STDIN_FILENO, &c, 1);
  if (nread == -1 && errno != EAGAIN) {
    Die("read");
  }
  if (errno == EAGAIN || nread == 0) {
    return -1;
  }
  return c;
}

void TerminalManager::ClearInput() {
  int nread;
  char buffer[4096];
  while ((nread = read(STDIN_FILENO, buffer, 4096)) == 4096) {}
  if (nread == -1 && errno != EAGAIN) {
    Die("read");
  }
}

void TerminalManager::EnableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios_) == -1) {
    Die("tcgetattr");
  }

  termios raw = orig_termios_;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1; // Read timeout

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    Die("tcsetattr");
  }
}

void TerminalManager::DisableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_) == -1) {
    Die("tcsetattr");
  }
}

void TerminalManager::HideCursor() {
  Write("\x1b[?25l", 6, true);
}

void TerminalManager::ShowCursor() {
  Write("\x1b[?25h", 6, true);
}

void TerminalManager::ClearScreen() {
  Write("\x1b[2J", 4, true);
}

void TerminalManager::Die(const char* str) {
  ClearScreen();
  SetCursor();

  perror(str);
  throw std::system_error(make_error_code(std::errc(errno)));
}

bool TerminalManager::Write(const char *s, const int str_len, const bool die) {
  if (write(STDOUT_FILENO, s, str_len) != str_len) {
    if (die) {
      Die("write");
    } else {
      return false;
    }
  }
  return true;
}

int TerminalManager::GetCursorPosition(int& rows, int& cols) {
  char buf[32];
  unsigned int i = 0;
  if (Write("\x1b[6n", 4)) {
    return -1;
  }
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) {
      break;
    }
    if (buf[i] == 'R') {
      break;
    }
    ++i;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') {
    return -1;
  }

  if (sscanf(&buf[2], "%d;%d", &rows, &cols) != 2) {
    return -1;
  }
  return 0;
}

int TerminalManager::GetWindowSize() {
  winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (Write("\x1b[999C\x1b[999B", 12)) {
      return -1;
    }
    return GetCursorPosition(terminal_rows_, terminal_cols_);
  } else {
    terminal_rows_ = ws.ws_row;
    terminal_cols_ = ws.ws_col;
    return 0;
  }
}

int TerminalManager::GetWindowSize(int& rows, int& cols) {
  GetWindowSize();
  rows = terminal_rows_;
  cols = terminal_cols_;
}

void TerminalManager::SetCursor(const int row, const int col) {
  char str[32];
  sprintf(str, "\x1b[%d;%dH", row, col);
  Write(str, strlen(str), true);
}

void TerminalManager::MoveCursorUp(const int steps) {
  char str[32];
  sprintf(str, "\x1b[%dA", steps);
  Write(str, strlen(str), true);
}

void TerminalManager::MoveCursorDown(const int steps) {
  char str[32];
  sprintf(str, "\x1b[%dB", steps);
  Write(str, strlen(str), true);
}

void TerminalManager::MoveCursorLeft(const int steps) {
  char str[32];
  sprintf(str, "\x1b[%dD", steps);
  Write(str, strlen(str), true);
}

void TerminalManager::MoveCursorRight(const int steps) {
  char str[32];
  sprintf(str, "\x1b[%dC", steps);
  Write(str, strlen(str), true);
}