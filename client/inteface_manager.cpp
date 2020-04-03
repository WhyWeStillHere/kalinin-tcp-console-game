#include "../lib/send_command.h"
#include "interface_manager.h"
#include <cctype>

InterfaceManager::InterfaceManager()
  : in_interface_(terminal_), out_interface_(terminal_) {
 terminal_.Init();
}

void InterfaceManager::Start() {
    out_interface_.DrawWelcomeScreen();
    in_interface_.PressAnyKey();
}

std::string InterfaceManager::AskLogin(const int MAX_LOGIN_SIZE) {
  std::string login;
  out_interface_.DrawLoginScreen();
  login = ReadWriteCursor(MAX_LOGIN_SIZE, true, true, LOGIN_SCREEN);
  return login;
}

CommandToManager InterfaceManager::MainServerPage(std::vector<RoomInfo>& room_list) {
  out_interface_.DrawRoomList(room_list);

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
      out_interface_.WriteError(" Incorrect Command", MAIN_SERVER_SCREEN);
    }
  }
}

int InterfaceManager::GetRoomId() {
  out_interface_.ReadRoomId();
  std::string out = ReadWriteCursor(4, true, false, MAIN_SERVER_SCREEN);
  return std::stoi(out);
}

std::string InterfaceManager::ReadWriteCursor(int MAX_INPUT_SIZE, const bool digit_fl, const bool letter_fl,
    const ScreenScene scene) {
  std::string out;

  while (1) {
    int c = in_interface_.ReadLetter();
    switch (c) {
    case DEL_KEY:
      if (!out.empty()) {
        out.pop_back();
        out_interface_.DeleteSymbol();
      }
      break;
    case ENTER_KEY:
      return out;
    default:
      if (((isalpha(c) && letter_fl) || (isdigit(c) && digit_fl))
          && (out.size() < MAX_INPUT_SIZE || MAX_INPUT_SIZE == -1)) {
        out.push_back(c);
        out_interface_.WriteSymbol(c);
      } else {
        if (!((isalpha(c) && letter_fl) || (isdigit(c) && digit_fl))) {
          out_interface_.WriteError("Incorrect Symbol", scene);
        } else {
          out_interface_.WriteError("Too Much Symbols", scene);
        }
      }
      break;
    }
  }
}