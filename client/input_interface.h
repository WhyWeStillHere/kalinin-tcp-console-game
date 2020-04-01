#include "terminal_manager.h"

class InputInterface {
public:
  explicit InputInterface(TerminalManager& terminal);

  void PressAnyKey();
private:
  TerminalManager& terminal_;
};