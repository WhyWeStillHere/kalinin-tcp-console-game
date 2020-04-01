#include "interface_manager.h"

InterfaceManager::InterfaceManager()
  : in_interface_(terminal_), out_interface_(terminal_) {
 terminal_.Init();
}

void InterfaceManager::Start() {
  while (1) {
    out_interface_.DrawWelcomeScreen();
    in_interface_.PressAnyKey();
  }
}