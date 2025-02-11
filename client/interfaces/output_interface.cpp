#include "output_interface.h"
#include "../../lib/game_lib/game_objects/game_object.h"
#include <cstring>
#include <sstream>
#include <string>

OutputInterface::OutputInterface(TerminalManager &terminal) : terminal_(terminal) {
  terminal_.GetWindowSize(rows_, cols_);
  painter_ = new Painter(terminal_, rows_, cols_);
  painter_->HideCursor();
}

OutputInterface::~OutputInterface() {
  delete painter_;
}

int OutputInterface::CheckWindowResize() {
  int rows, cols;
  terminal_.GetWindowSize(rows, cols);
  if  (rows != rows_ || cols != cols_) {
    delete painter_;
    rows_ = rows;
    cols_ = cols;
    painter_ = new Painter(terminal_, rows_, cols_);
    return 1;
  }
  return 0;
}

void OutputInterface::DrawWelcomeScreen() {
  ClearScreen();

  painter_->DrawRectangle(0.25, 0.333, 0.75, 0.666,
      ':', '=');

  char str_buffer[4096];
  strcpy(str_buffer, "Welcome To The Game");
  painter_->DrawStringCenter(0.5, 0.4, str_buffer, strlen(str_buffer));
  strcpy(str_buffer, "Press any key");
  painter_->DrawStringCenter(0.5, 0.5, str_buffer, strlen(str_buffer));

  painter_->ApplyDrawing();
}

void OutputInterface::DrawGoodbyeScreen() {
  ClearScreen();
  painter_->HideCursor();
  painter_->DrawRectangle(0.25, 0.333, 0.75, 0.666,
                          ':', '=');
  char str_buffer[4096];
  strcpy(str_buffer, "Thanks for playing!");
  painter_->DrawStringCenter(0.5, 0.4, str_buffer, strlen(str_buffer));
  strcpy(str_buffer, "Press any key");
  painter_->DrawStringCenter(0.5, 0.5, str_buffer, strlen(str_buffer));
  painter_->ApplyDrawing();
}

void OutputInterface::DrawErrorScreen(const std::string& error) {
  ClearScreen();
  painter_->HideCursor();

  painter_->DrawRectangle(0.25, 0.333, 0.75, 0.666,
                          ':', '=');

  char str_buffer[4096];
  strcpy(str_buffer, "Sorry error occurred");
  painter_->DrawStringCenter(0.5, 0.4, str_buffer, strlen(str_buffer));
  strcpy(str_buffer, "Error: " );
  strcpy(str_buffer + strlen(str_buffer), error.data());
  painter_->DrawStringCenter(0.5, 0.48, str_buffer, strlen(str_buffer));
  strcpy(str_buffer, "Press any key to quit" );
  painter_->DrawStringCenter(0.5, 0.56, str_buffer, strlen(str_buffer));

  painter_->ApplyDrawing();
}

void OutputInterface::ClearScreen() {
  if (!CheckWindowResize()) {
    painter_->ClearScreen();
  }
  painter_->DrawFrame();
}

void OutputInterface::DrawLoginScreen() {
  ClearScreen();

  painter_->DrawRectangle(0.25, 0.40, 0.75, 0.60,
                          ':', '=');

  char str_buffer[4096];
  sprintf(str_buffer, "You can press Ctrl+q to quit");
  painter_->DrawStringCenter(0.5, 0.24, str_buffer, strlen(str_buffer));

  sprintf(str_buffer, "Write your login:");
  painter_->DrawStringCenter(0.5, 0.43, str_buffer, strlen(str_buffer));
  painter_->ApplyDrawing();

  painter_->MoveCursor(0.3, 0.52);
  painter_->ShowCursor();
}

void OutputInterface::WriteSymbol(const char c) {
  painter_->Set(c);
  painter_->ApplyDrawing();
  painter_->MoveCursor(Cursor::Direction::RIGHT);
}

void OutputInterface::DeleteSymbol() {
  painter_->MoveCursor(Cursor::Direction::LEFT);
  painter_->Remove();
  painter_->ApplyDrawing();
}

void OutputInterface::DrawRoomList(std::vector<RoomInfo>& room_list) {
  painter_->HideCursor();
  painter_->ClearScreen();

  char str_buffer[128];

  int x_max = painter_->GetHorizontalCoord(0.999);
  int y_max = painter_->GetVerticalCoord(0.999);

  int cross_vert_line = painter_->GetHorizontalCoord(0.66);
  int cross_horizontal_line = painter_->GetVerticalCoord(0.66);

  painter_->DrawHorizontalLine(cross_horizontal_line, cross_vert_line, x_max, '_');
  painter_->DrawHorizontalLine(cross_horizontal_line + 1, cross_vert_line, x_max, '_');
  painter_->DrawVerticalLine(cross_vert_line, 0, y_max, '|');

  // Draw room list
  painter_->DrawRectangle(0, 0, cross_vert_line, 2);
  sprintf(str_buffer, " Available rooms: ");
  painter_->DrawString(1, 1, str_buffer, strlen(str_buffer));
  DrawRooms(0, 2, cross_vert_line, y_max, room_list);

  // Draw command list
  painter_->DrawRectangle(cross_vert_line, 0, x_max, 2);
  sprintf(str_buffer, " Available commands: ");
  painter_->DrawString(cross_vert_line + 1, 1, str_buffer, strlen(str_buffer));
  std::vector<std::string> commands;
  commands.emplace_back(" [0] Refresh Rooms");
  commands.emplace_back(" [1] Join Room");
  commands.emplace_back(" [2] Create Room");
  DrawList(cross_vert_line, 2, x_max, cross_horizontal_line, commands);

  // Draw Action section
  sprintf(str_buffer, "User Command: ");
  painter_->DrawString(cross_vert_line + 2, cross_horizontal_line + 3, str_buffer, strlen(str_buffer));
  painter_->MoveCursor(cross_vert_line + 2 + strlen(str_buffer), cross_horizontal_line + 3);

  painter_->DrawFrame();

  painter_->ApplyDrawing();
}

void OutputInterface::DrawList(const int x1, const int y1, const int x2, const int y2,
    const std::vector<std::string>& lines) {
  int y_shift = 0;
  for (const auto& line: lines) {
    painter_->DrawRectangle(x1, y1 + y_shift, x2, y1 + y_shift + 2);
    painter_->DrawString(x1 + 1, y1 + y_shift + 1, line.data(), line.size());
    y_shift += 2;
  }
}

void OutputInterface::DrawRooms(const int x1, const int y1, const int x2,
    const int y2, const std::vector<RoomInfo> &room_list) {
  std::vector<std::string> lines;
  for (const auto& room: room_list) {
    std::ostringstream ss;
    ss << " Room id: " << room.id << "       Participants: " << room.participant_num;
    lines.push_back(ss.str());
  }

  DrawList(x1, y1, x2, y2, lines);
}

void OutputInterface::ReadRoomId() {
  int cross_vert_line = painter_->GetHorizontalCoord(0.66);
  int cross_horizontal_line = painter_->GetVerticalCoord(0.66);

  char str_buffer[32];
  sprintf(str_buffer, "Room id: ");
  painter_->DrawString(cross_vert_line + 2, cross_horizontal_line + 4, str_buffer, strlen(str_buffer));
  painter_->MoveCursor(cross_vert_line + 2 + strlen(str_buffer), cross_horizontal_line + 4);
  painter_->ShowCursor();
  painter_->ApplyDrawing();
}

void OutputInterface::WriteError(const std::string &error) {
  std::string out = "Error: " + error;
  switch (current_scene_) {
  case LOGIN_SCREEN:
    painter_->DrawStringCenter(0.5, 0.75, out.data(), out.size());
    painter_->ApplyDrawing();
    painter_->RemoveStringCenter(0.5, 0.2, out.size());
    break;
  case MAIN_SERVER_SCREEN:
    painter_->DrawStringCenter(0.2, 0.9, out.data(), out.size());
    painter_->ApplyDrawing();
    painter_->RemoveStringCenter(0.2, 0.9, out.size());
    break;
  case LOBBY_SCREEN:
    painter_->DrawString(0.43, 0.84, out.data(), out.size());
    painter_->ApplyDrawing();
    painter_->RemoveString(0.43, 0.84, out.size());
    break;
  }
}

void OutputInterface::DrawLobbyScreen(const std::vector<PlayerInfo>& player_list,
                                      const bool is_host) {
  if (player_list.empty()) {
    throw std::logic_error("Incorrect player list");
  }
  painter_->HideCursor();
  painter_->ClearScreen();

  char str_buffer[128];

  int x_max = painter_->GetHorizontalCoord(0.999);
  int y_max = painter_->GetVerticalCoord(0.999);

  int cross_vert_line = painter_->GetHorizontalCoord(0.4);
  int cross_horizontal_line = painter_->GetVerticalCoord(0.66);

  painter_->DrawHorizontalLine(cross_horizontal_line, cross_vert_line, x_max, '_');
  painter_->DrawHorizontalLine(cross_horizontal_line + 1, cross_vert_line, x_max, '_');
  painter_->DrawVerticalLine(cross_vert_line, 0, y_max, '|');


  painter_->DrawRectangle(0, 0, cross_vert_line, 2);
  sprintf(str_buffer, " Host: %s", player_list.front().login.data());
  painter_->DrawString(1, 1, str_buffer, strlen(str_buffer));

  // Draw player list
  painter_->DrawRectangle(0, 2, cross_vert_line, 4, '|', '=');
  sprintf(str_buffer, " Current players: ");
  painter_->DrawString(1, 3, str_buffer, strlen(str_buffer));
  DrawPlayers(0, 4, cross_vert_line, y_max, player_list);

  if (is_host) {
    // Draw command list
    painter_->DrawRectangle(cross_vert_line, 0, x_max, 2);
    sprintf(str_buffer, " Available commands: ");
    painter_->DrawString(cross_vert_line + 1, 1, str_buffer, strlen(str_buffer));
    std::vector<std::string> commands;
    commands.emplace_back(" [0] Start game");
    DrawList(cross_vert_line, 2, x_max, cross_horizontal_line, commands);

    // Draw Action section
    sprintf(str_buffer, "User Command: ");
    painter_->DrawString(cross_vert_line + 2, cross_horizontal_line + 3, str_buffer, strlen(str_buffer));
    painter_->MoveCursor(cross_vert_line + 2 + strlen(str_buffer), cross_horizontal_line + 3);
  }

  painter_->DrawFrame();

  painter_->ApplyDrawing();
}

bool OutputInterface::DrawGameScreen(const GameInfo &game_info,
                                     const std::vector<PlayerInfo>& players,
                                     const int player_id,
                                     const size_t filed_of_view) {
  painter_->HideCursor();
  painter_->ClearScreen();

  char str_buffer[128];

  int x_max = painter_->GetHorizontalCoord(0.999);
  int y_max = painter_->GetVerticalCoord(0.999);

  int cross_vert_line = painter_->GetHorizontalCoord(0.4);
  int cross_horizontal_line = painter_->GetVerticalCoord(0.66);

  painter_->DrawHorizontalLine(cross_horizontal_line, cross_vert_line, x_max, '_');
  painter_->DrawHorizontalLine(cross_horizontal_line + 1, cross_vert_line, x_max, '_');
  painter_->DrawVerticalLine(cross_vert_line, 0, y_max, '|');

  sprintf(str_buffer,"Movement - WASD or Arrow Keys");
  painter_->DrawString(cross_vert_line + 2,cross_horizontal_line + 3,
                             str_buffer, strlen(str_buffer));
  sprintf(str_buffer,"Shoot - Spacebar or K");
  painter_->DrawString(cross_vert_line + 2,cross_horizontal_line + 4,
                       str_buffer, strlen(str_buffer));

  painter_->DrawRectangle(0, 0, cross_vert_line, 2);
  sprintf(str_buffer, " Host: %s", players.front().login.data());
  painter_->DrawString(1, 1, str_buffer, strlen(str_buffer));

  // Draw player list
  painter_->DrawRectangle(0, 2, cross_vert_line, 4, '|', '=');
  sprintf(str_buffer, " Current players: ");
  painter_->DrawString(1, 3, str_buffer, strlen(str_buffer));
  std::vector<std::string> lines;
  int alive_players = 0;
  int player_x = -1, player_y = -1;
  for (int i = 1; i < players.size(); ++i) {
    for (const auto& player: game_info.players_info) {
      if (player.id == players[i].id) {
        std::ostringstream ss;
        if (player.health_value != 0) {
          ss << " Player " << players[i].login << " HP: " << player.health_value;
          ++alive_players;
        } else {
          ss << " Player " << players[i].login << " Dead";
        }
        lines.push_back(ss.str());
        if (player_id == player.id) {
          player_x = player.x_coord;
          player_y = player.y_coord;
        }
        break;
      }
    }
  }
  if (alive_players <= 1) {
    lines.clear();
    for (int i = 1; i < players.size(); ++i) {
      for (const auto& player: game_info.players_info) {
        if (player.id == players[i].id) {
          std::ostringstream ss;
          if (player.health_value != 0) {
            ss << " Player " << players[i].login << " Winner";
          } else {
            ss << " Player " << players[i].login << " Loser";
          }
          lines.push_back(ss.str());
          if (player_id == player.id) {
            player_x = player.x_coord;
            player_y = player.y_coord;
          }
          break;
        }
      }
    }
  }
  DrawList(0, 4, cross_vert_line, y_max, lines);

  // Draw map
  DrawMap(cross_vert_line + 2 , 2, game_info.map, player_x, player_y,
          filed_of_view);

  painter_->DrawFrame();
  painter_->ApplyDrawing();
  return alive_players <= 1;
}

void OutputInterface::DrawMap(int x, int y, const GameMapInfo& map_info,
                              int player_x, int player_y,
                              const size_t filed_of_view) {
  if (player_x == -1 || player_y == -1
      || map_info.map[player_x][player_y] != GameObjectType::PLAYER) {
    for (int i = 0; i < map_info.map.size(); ++i) {
      for (int j = 0; j < map_info.map[i].size(); ++j) {
        switch (map_info.map[i][j]) {
        case GameObjectType::PLAYER:
          painter_->Set(x + i, y + j, '@');
          break;
        case GameObjectType::VOID:
          painter_->Set(x + i, y + j, ' ');
          break;
        case GameObjectType::WALL:
          painter_->Set(x + i, y + j, '#');
          break;
        case GameObjectType::PROJECTILE:
          painter_->Set(x + i, y + j, 'o');
          break;
        default:
          painter_->Set(x + i, y + j, '?');
          break;
        }
      }
    }
  } else {
    for (int i = std::max(0, player_x - static_cast<int>(filed_of_view)), x_pos = 0;
         i < std::min(map_info.map.size(), player_x + filed_of_view); ++i, ++x_pos) {
      for (int j = std::max(0, player_y - static_cast<int>(filed_of_view)), y_pos = 0;
           j < std::min(map_info.map[i].size(), player_y + filed_of_view); ++j, ++y_pos) {
        switch (map_info.map[i][j]) {
        case GameObjectType::PLAYER:
          if (i == player_x && j == player_y) {
            painter_->Set(x + x_pos, y + y_pos, 'P');
          } else {
            painter_->Set(x + x_pos, y + y_pos, '@');
          }
          break;
        case GameObjectType::VOID:
          painter_->Set(x + x_pos, y + y_pos, ' ');
          break;
        case GameObjectType::WALL:
          painter_->Set(x + x_pos, y + y_pos, '#');
          break;
        case GameObjectType::PROJECTILE:
          painter_->Set(x + x_pos, y + y_pos, 'o');
          break;
        default:
          painter_->Set(x + x_pos, y + y_pos, '?');
          break;
        }
      }
    }
  }
}

void OutputInterface::DrawPlayers(const int x1, const int y1, const int x2,
                                const int y2, const std::vector<PlayerInfo>& player_list) {
  std::vector<std::string> lines;
  for (int i = 1; i < player_list.size(); ++i) {
    std::ostringstream ss;
    ss << " Player " << player_list[i].login;
    lines.push_back(ss.str());
  }

  DrawList(x1, y1, x2, y2, lines);
}

void OutputInterface::SetScene(const ScreenScene scene) {
  current_scene_ = scene;
}

ScreenScene OutputInterface::CurrentScene() {
  return current_scene_;
}
