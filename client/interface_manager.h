#include "terminal_manager.h"
#include "output_interface.h"
#include "input_interface.h"
#include "../lib/send_command.h"

class InterfaceManager {
public:
  InterfaceManager();

  void Start();
  std::string AskLogin(int MAX_LOGIN_SIZE = 32);
  CommandToManager MainServerPage(std::vector<RoomInfo>& room_list);
  int GetRoomId();

private:
  std::string ReadWriteCursor(int MAX_INPUT_SIZE = -1, bool digit_fl = true,
      bool letter_fl = true, ScreenScene scene = UNKNOWN_SCREEN);

  TerminalManager terminal_;
  OutputInterface out_interface_;
  InputInterface in_interface_;
};