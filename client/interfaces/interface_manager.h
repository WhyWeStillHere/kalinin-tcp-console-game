#include "../../lib/info_structures/game_info.h"
#include "../../lib/info_structures/game_map_info.h"
#include "../../lib/network_utilities/send_command.h"
#include "../terminal_manager.h"
#include "input_interface.h"
#include "output_interface.h"

class InterfaceManager {
public:
  InterfaceManager();

  void Start();
  std::string AskLogin(int MAX_LOGIN_SIZE = 32);
  CommandToManager MainServerPage(std::vector<RoomInfo>& room_list,
      const std::string& err = "");
  int GetRoomId();
  bool LobbyPage(std::vector<PlayerInfo>& player_list, bool is_host = false);
  void InitGamePage(const GameInfo &game_info,
                    const std::vector<PlayerInfo>& players, int player_id,
                    size_t field_of_view);
  CommandToGame UpdateGamePage(const GameInfo &game_info,
                      const std::vector<PlayerInfo>& players, int player_id,
                      size_t field_of_view);

  void ErrorScreen(const std::string& err);
  void WriteError(const std::string& err);
  void GoodbyeScreen();

private:
  std::string ReadWriteCursor(int MAX_INPUT_SIZE = -1, bool digit_fl = true,
      bool letter_fl = true);

  TerminalManager terminal_;
  OutputInterface out_interface_;
  InputInterface in_interface_;
};