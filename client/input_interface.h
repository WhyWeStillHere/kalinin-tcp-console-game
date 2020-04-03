#include "terminal_manager.h"

enum editorKey {
  DEL_KEY = 1000,
  ENTER_KEY,
  UNKNOWN_KEY
};

class InputInterface {
public:
  explicit InputInterface(TerminalManager& terminal);

  void PressAnyKey();
  int ReadLetter();
private:
  TerminalManager& terminal_;
};