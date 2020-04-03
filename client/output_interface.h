#include "painter.h"
#include "terminal_manager.h"
#include "../lib/room_info.h"

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
  void ReadRoomId();

  void WriteError(const std::string& error, ScreenScene screen);

private:
  void DrawList(int x1, int y1, int x2, int y2, std::vector<std::string>& lines);
  void DrawRooms(int x1, int y1, int x2, int y2, std::vector<RoomInfo>& room_list);

  TerminalManager& terminal_;
  Painter* painter_ = nullptr;
  int rows_, cols_;
};