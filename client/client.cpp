#include "client.h"
#include "../lib/info_structures/room_info.h"
#include "../lib/network_utilities/send_command.h"
#include "../lib/network_utilities/tcp_utilities.h"
#include "../lib/sleep.h"

#include <sys/ioctl.h>
#include <iostream>
#include <vector>

volatile sig_atomic_t Client::exit_flag_ = 0;

Client::Client() {
  ConfigureSignals();
}

Client::~Client() {
  if (socket_ != -1) {
    shutdown(socket_, SHUT_RDWR);
    char buffer[4096];
    int ret;
    do {
      ret = read(socket_, buffer, sizeof(buffer));
    } while (ret > 0);
    close(socket_);
  }
}

std::vector<RoomInfo> Client::LoadRooms() {
  return read_vector<RoomInfo>(socket_);
}

std::vector<PlayerInfo> Client::LoadPlayers() {
  return read_vector<PlayerInfo>(socket_);
}

void Client::JoinRoom(int room_id, const std::vector<RoomInfo> &rooms_info) {
  bool correct_id = false;
  for (const auto& room: rooms_info) {
    if (room.id == room_id) {
      correct_id = true;
      break;
    }
  }
  if (correct_id) {
    auto room_id_uint = static_cast<uint32_t>(room_id);
    WriteCommand(CommandToManager::JOIN_ROOM, socket_);
    write_uint32(socket_, &room_id_uint);
    write_string(socket_, login_);
    std::vector<PlayerInfo> players_info;
    players_info = LoadPlayers();
    interface_manager_.LobbyPage(players_info);
    while (true) {
      auto command = ReadCommand<CommandToPlayer>(socket_);
      if (command == START_GAME) {
        break;
      }
      players_info = LoadPlayers();
      interface_manager_.LobbyPage(players_info);
    }
    GameInfo game_info;
    game_info.Read(socket_);
    int player_id = -1;
    for (int i = 0; i < players_info.size(); ++i) {
      for (const auto& player: game_info.players_info) {
        if (player.id == players_info[i].id && players_info[i].login == login_) {
          player_id = player.id;
          break;
        }
      }
    }
    interface_manager_.InitGamePage(game_info, players_info, player_id,
                                    field_of_view_);
    int count = 0;
    while (true) {
      ioctl(socket_, FIONREAD, &count);
      if (count != 0) {
        game_info.Read(socket_);
      }
      CommandToGame interface_command(interface_manager_.UpdateGamePage(
          game_info, players_info, player_id, field_of_view_));
      if (interface_command == GAME_ENDED) {
        sleep_millisecond(6000);
        break;
      }
      if (interface_command != UNKNOWN_INPUT) {
        WriteCommand<CommandToGame>(interface_command, socket_);
      }
    }
    end_of_game = true;
    interface_manager_.GoodbyeScreen();
  } else {
    last_error_ = "Incorrect room ID";
  }
}

void Client::CreateRoom() {
  WriteCommand(CommandToManager::CREATE_ROOM, socket_);
  write_string(socket_, login_);
  std::vector<PlayerInfo> players_info = LoadPlayers();
  interface_manager_.LobbyPage(players_info, true);
  int count = 0;
  while (true) {
    ioctl(socket_, FIONREAD, &count);
    if (count != 0) {
      auto command = ReadCommand<CommandToPlayer>(socket_);
      players_info = LoadPlayers();
    }
    sleep_millisecond(50);
    bool start_fl = interface_manager_.LobbyPage(players_info, true);
    if (start_fl) {
      if (players_info.size() < 3) {
        interface_manager_.WriteError("To few players, needed at least 2");
        sleep_millisecond(2000);
        continue;
      }
      break;
    }
  }
  WriteCommand<CommandToRoom>(START_GAME_HOST, socket_);
  GameInfo game_info;
  game_info.Read(socket_);
  int player_id = -1;
  for (int i = 0; i < players_info.size(); ++i) {
    for (const auto& player: game_info.players_info) {
      if (player.id == players_info[i].id && players_info[i].login == login_) {
        player_id = player.id;
        break;
      }
    }
  }
  interface_manager_.InitGamePage(game_info, players_info, player_id,
                                  field_of_view_);
  while (true) {
    ioctl(socket_, FIONREAD, &count);
    if (count != 0) {
      game_info.Read(socket_);
    }
    CommandToGame interface_command(interface_manager_.UpdateGamePage(
        game_info, players_info, player_id, field_of_view_));
    if (interface_command == GAME_ENDED) {
      sleep_millisecond(6000);
      break;
    }
    if (interface_command != UNKNOWN_INPUT) {
      WriteCommand<CommandToGame>(interface_command, socket_);
    }
  }
  end_of_game = true;
  interface_manager_.GoodbyeScreen();
}

void Client::Run(const char* ip_string, const int port) {
  try {
    interface_manager_.Start();
    login_ = interface_manager_.AskLogin();

    Connect(ip_string, port);

    std::vector<RoomInfo> rooms_info = LoadRooms();

    while (!end_of_game) {
      CommandToManager command = interface_manager_.MainServerPage(rooms_info, last_error_);
      last_error_.clear();
      if (command == JOIN_ROOM) {
        JoinRoom(interface_manager_.GetRoomId(), rooms_info);
        continue;
      } else if (command == REFRESH_ROOMS) {
        WriteCommand(CommandToManager::REFRESH_ROOMS, socket_);
        rooms_info = LoadRooms();
        continue;
      } else if (command == CREATE_ROOM) {
        CreateRoom();
        continue;
      }
    }
  } catch (int err_code) {
    if (err_code == -1) {
      return;
    }
    interface_manager_.ErrorScreen("Terminal error occurred");
  } catch (...) {
    interface_manager_.ErrorScreen("Server error occurred");
  }
}

void Client::ExitHandler(int signum) {
  exit_flag_ = 1;
}

void Client::ConfigureSignals() {
  struct sigaction action_int;
  memset(&action_int, 0, sizeof(action_int));
  action_int.sa_handler = Client::ExitHandler;
  int ret;
  ret = sigaction(SIGTERM, &action_int, NULL);
  if (ret < 0) {
    throw std::runtime_error("Error while sigaction()");
  }
  ret = sigaction(SIGINT, &action_int, NULL);
  if (ret < 0) {
    throw std::runtime_error("Error while sigaction()");
  }
}

void Client::Init() {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    throw std::runtime_error("Unable to bind socket");
  }
}

void Client::Connect(const char *ip_string, const int port) {
  struct in_addr server_in_addr;
  inet_aton(ip_string, &server_in_addr);

  struct sockaddr_in server_sockaddr;
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_addr = server_in_addr;
  server_sockaddr.sin_port = htons(port);

  int ret = connect(socket_, (const struct sockaddr*)&server_sockaddr,
      sizeof(struct sockaddr_in));
  if (ret < 0) {
    throw std::runtime_error("connect() error");
  }
}
