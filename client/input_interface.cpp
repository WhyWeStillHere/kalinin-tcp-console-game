#include "input_interface.h"

InputInterface::InputInterface(TerminalManager &terminal) : terminal_(terminal) {}

void InputInterface::PressAnyKey() {
  terminal_.ReadKey();
}
