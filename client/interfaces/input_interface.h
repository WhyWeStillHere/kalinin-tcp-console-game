#include "../terminal_manager.h"

enum editorKey {
  DEL_KEY = 1000,
  ENTER_KEY,
  UNKNOWN_KEY,
  ARROW_UP,
  ARROW_DOWN,
  ARROW_LEFT,
  ARROW_RIGHT
};

class InputInterface {
public:
  explicit InputInterface(TerminalManager& terminal);

  void PressAnyKey();
  int ReadLetter();
  int ReadLetterNonBlock();
private:
  int ParseLetter(char letter);
  TerminalManager& terminal_;
};