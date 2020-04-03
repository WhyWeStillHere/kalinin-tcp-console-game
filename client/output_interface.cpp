#include "output_interface.h"
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

void OutputInterface::DrawList(const int x1, const int y1, const int x2, const int y2, std::vector<std::string>& lines) {
  int y_shift = 0;
  for (const auto& line: lines) {
    painter_->DrawRectangle(x1, y1 + y_shift, x2, y1 + y_shift + 2);
    painter_->DrawString(x1 + 1, y1 + y_shift + 1, line.data(), line.size());
    y_shift += 2;
  }
}

void OutputInterface::DrawRooms(const int x1, const int y1, const int x2, const int y2, std::vector<RoomInfo> &room_list) {
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

void OutputInterface::WriteError(const std::string &error, ScreenScene screen) {
  std::string out = "Error: " + error;
  switch (screen) {
  case LOGIN_SCREEN:
    painter_->DrawStringCenter(0.5, 0.2, out.data(), out.size());
    painter_->ApplyDrawing();
    painter_->RemoveStringCenter(0.5, 0.2, out.size());
    break;
  case MAIN_SERVER_SCREEN:
    painter_->DrawStringCenter(0.2, 0.9, out.data(), out.size());
    painter_->ApplyDrawing();
    painter_->RemoveStringCenter(0.2, 0.9, out.size());
    break;
  }
}
