#include "painter.h"
#include "terminal_manager.h"

class OutputInterface {
public:
  explicit OutputInterface(TerminalManager& terminal);
  ~OutputInterface();

  void DrawWelcomeScreen();
  void CheckWindowResize();
  /*void DrawLoginScreen();
  void DrawRoomList();*/

private:
  TerminalManager& terminal_;
  Painter* painter_ = nullptr;
  int rows_, cols_;
};