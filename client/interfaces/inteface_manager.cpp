#include "../../lib/network_utilities/send_command.h"
#include "interface_manager.h"
#include <cctype>

InterfaceManager::InterfaceManager()
  : in_interface_(terminal_), out_interface_(terminal_) {
 terminal_.Init();
}

void InterfaceManager::Start() {
    out_interface_.SetScene(ScreenScene::WELCOME_SCREEN);
    out_interface_.DrawWelcomeScreen();
    in_interface_.PressAnyKey();
}

std::string InterfaceManager::AskLogin(const int MAX_LOGIN_SIZE) {
  out_interface_.SetScene(ScreenScene::LOGIN_SCREEN);
  std::string login;
  out_interface_.DrawLoginScreen();
  login = ReadWriteCursor(MAX_LOGIN_SIZE, true, true);
  return login;
}

CommandToManager InterfaceManager::MainServerPage(std::vector<RoomInfo>& room_list,
    const std::string& err) {
  out_interface_.SetScene(ScreenScene::MAIN_SERVER_SCREEN);
  out_interface_.DrawRoomList(room_list);
  if (!err.empty()) {
    out_interface_.WriteError(err);
  }

  std::string out;

  while (true) {
    int c = in_interface_.ReadLetter();
    switch (c) {
    case '0':
      out_interface_.WriteSymbol(c);
      return REFRESH_ROOMS;
    case '1':
      out_interface_.WriteSymbol(c);
      return JOIN_ROOM;
    case '2':
      out_interface_.WriteSymbol(c);
      return CREATE_ROOM;
    default:
      out_interface_.WriteError(" Incorrect Command");
    }
  }
}

bool InterfaceManager::LobbyPage(std::vector<PlayerInfo> &player_list, bool is_host) {
  out_interface_.SetScene(ScreenScene::LOBBY_SCREEN);
  out_interface_.DrawLobbyScreen(player_list, is_host);
  if (is_host) {
    int c = in_interface_.ReadLetterNonBlock();
    switch (c) {
      case '0':
        out_interface_.WriteSymbol(c);
        return true;
      default:
        if (c > 0) {
          out_interface_.WriteError(" Incorrect Command");
        }
        return false;
    }
  }
  return false;
}

void InterfaceManager::InitGamePage(const GameInfo &game_info,
                                    const std::vector<PlayerInfo>& players,
                                    int player_id, const size_t field_of_view) {
  out_interface_.DrawGameScreen(game_info, players, player_id, field_of_view);
}

CommandToGame InterfaceManager::UpdateGamePage(const GameInfo &game_info,
                                               const std::vector<PlayerInfo> &players,
                                               const int player_id,
                                               const size_t field_of_view) {
  out_interface_.SetScene(ScreenScene::GAME_SCREEN);
  bool finish = out_interface_.DrawGameScreen(game_info, players, player_id, field_of_view);
  if (finish) {
    return GAME_ENDED;
  }
  while (true) {
    int c = in_interface_.ReadLetterNonBlock();
    switch (c) {
    case 'W':
    case 'w':
      return MOVE_UP;
    case 'S':
    case 's':
      return MOVE_DOWN;
    case 'D':
    case 'd':
      return MOVE_RIGHT;
    case 'A':
    case 'a':
      return MOVE_LEFT;
    case 'K':
    case 'k':
      return SHOOT;
    default:
      return UNKNOWN_INPUT;
    }
  }
}


int InterfaceManager::GetRoomId() {
  out_interface_.ReadRoomId();
  std::string out = ReadWriteCursor(4, true, false);
  return std::stoi(out);
}

std::string InterfaceManager::ReadWriteCursor(int MAX_INPUT_SIZE,
    const bool digit_fl, const bool letter_fl) {
  std::string out;

  while (true) {
    int c = in_interface_.ReadLetter();
    switch (c) {
    case DEL_KEY:
      if (!out.empty()) {
        out.pop_back();
        out_interface_.DeleteSymbol();
      }
      break;
    case ENTER_KEY:
      if (out.empty()) {
        break;
      }
      return out;
    default:
      if (((isalpha(c) && letter_fl) || (isdigit(c) && digit_fl))
          && (out.size() < MAX_INPUT_SIZE || MAX_INPUT_SIZE == -1)) {
        out.push_back(c);
        out_interface_.WriteSymbol(c);
      } else {
        if (!((isalpha(c) && letter_fl) || (isdigit(c) && digit_fl))) {
          out_interface_.WriteError("Incorrect Symbol");
        } else {
          out_interface_.WriteError("Too Much Symbols");
        }
      }
      break;
    }
  }
}

void InterfaceManager::ErrorScreen(const std::string& err) {
  try {
    out_interface_.SetScene(ScreenScene::END_SCREEN);
    out_interface_.DrawErrorScreen(err);
    in_interface_.PressAnyKey();
  } catch(...) {
  }
}

void InterfaceManager::GoodbyeScreen() {
  try {
    out_interface_.SetScene(ScreenScene::END_SCREEN);
    out_interface_.DrawGoodbyeScreen();
    in_interface_.PressAnyKey();
  } catch(...) {
  }
}

void InterfaceManager::WriteError(const std::string &err) {
  out_interface_.WriteError(err);
}