#include "terminal_manager.h"
#include "output_interface.h"
#include "input_interface.h"

class InterfaceManager {
public:
  InterfaceManager();

  void Start();

private:
  TerminalManager terminal_;
  OutputInterface out_interface_;
  InputInterface in_interface_;
};