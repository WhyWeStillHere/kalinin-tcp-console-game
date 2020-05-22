#include "input_interface.h"

#define CTRL_KEY(k) ((k) & 0x1f) // K & 00011111

InputInterface::InputInterface(TerminalManager &terminal) : terminal_(terminal) {}

void InputInterface::PressAnyKey() {
  char c = terminal_.ReadKeyBlock();

  if (c == CTRL_KEY('q')) {
    throw -1;
  }
}
int InputInterface::ParseLetter(const char letter) {
  if (letter == CTRL_KEY('q')) {
    throw -1;
  }

  if (letter == '\x1b') {
    int seq[3];
    seq[0] = terminal_.ReadKey();
    if (seq[0] == -1) {
      return '\x1b';
    }
    seq[1] = terminal_.ReadKey();
    if (seq[1] == -1) {
      return '\x1b';
    }
    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        seq[2] = terminal_.ReadKey();
        if (seq[2] == -1) {
          return '\x1b';
        }
        if (seq[2] == '~') {
          switch (seq[1]) {
          case '3':
            return DEL_KEY;
          }
        }
      } else {
        switch (seq[1]) {
        case 'A':
          return ARROW_UP;
        case 'B':
          return ARROW_DOWN;
        case 'C':
          return ARROW_RIGHT;
        case 'D':
          return ARROW_LEFT;
        default:
          return '\x1b';
        }
      }
    }
    return UNKNOWN_KEY;
  } else if (letter == '\x7f') {
    return DEL_KEY;
  } else if (letter == 13) {
    return ENTER_KEY;
  } else {
    return letter;
  }
}

int InputInterface::ReadLetter() {
  char c = terminal_.ReadKeyBlock();
  return ParseLetter(c);
}

int InputInterface::ReadLetterNonBlock() {
  int c = terminal_.ReadKey();
  if (c == -1) {
    return -1;
  }
  return ParseLetter(c);
}
