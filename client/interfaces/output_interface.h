#include "../../lib/info_structures/game_info.h"
#include "../../lib/info_structures/player_info.h"
#include "../../lib/info_structures/player_object_info.h"
#include "../../lib/info_structures/room_info.h"
#include "../painter.h"
#include "../terminal_manager.h"

#include <string>

enum ScreenScene {
  WELCOME_SCREEN,
  LOGIN_SCREEN,
  MAIN_SERVER_SCREEN,
  LOBBY_SCREEN,
  GAME_SCREEN,
  UNKNOWN_SCREEN
};

class OutputInterface {
public:
  explicit OutputInterface(TerminalManager& terminal);
  ~OutputInterface();

  void DrawWelcomeScreen();
  int CheckWindowResize();
  void DrawLoginScreen();
  void WriteSymbol(char c);
  void DeleteSymbol();
  void ClearScreen();
  void DrawRoomList(std::vector<RoomInfo>& room_list);
  void DrawLobbyScreen(const std::vector<PlayerInfo>& player_list,
                       bool is_host = false);
  void ReadRoomId();
  void DrawGameScreen(const GameInfo& game_info,
                      const std::vector<PlayerInfo>& players,
                      int player_id);
  void DrawMap(int x, int y, const GameMapInfo& map_info,
               int player_x, int player_y);

  void WriteError(const std::string& error);

  void SetScene(ScreenScene scene);
  ScreenScene CurrentScene();

private:
  void DrawList(int x1, int y1, int x2, int y2,
      const std::vector<std::string>& lines);
  void DrawRooms(int x1, int y1, int x2, int y2,
      const std::vector<RoomInfo>& room_list);
  void DrawPlayers(int x1, int y1, int x2, int y2,
      const std::vector<PlayerInfo>& player_list);

  ScreenScene current_scene_ = UNKNOWN_SCREEN;
  TerminalManager& terminal_;
  Painter* painter_ = nullptr;
  int rows_, cols_;
};