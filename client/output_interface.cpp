#include "output_interface.h"
#include <cstring>

OutputInterface::OutputInterface(TerminalManager &terminal) : terminal_(terminal) {
  terminal_.GetWindowSize(rows_, cols_);
  painter_ = new Painter(terminal_, rows_, cols_);
}

OutputInterface::~OutputInterface() {
  delete painter_;
}

void OutputInterface::CheckWindowResize() {
  int rows, cols;
  terminal_.GetWindowSize(rows, cols);
  if  (rows != rows_ || cols != cols_) {
    delete painter_;
    rows_ = rows;
    cols_ = cols;
    painter_ = new Painter(terminal_, rows_, cols_);
  }
}

void OutputInterface::DrawWelcomeScreen() {
  CheckWindowResize();
  painter_->DrawFrame();
  painter_->DrawRectangle(0.25, 0.333, 0.75, 0.666,
      ':', '=');

  char str_buffer[4096];
  strcpy(str_buffer, "Welcome To The Game");
  painter_->DrawStringCenter(0.5, 0.4, str_buffer, strlen(str_buffer));
  strcpy(str_buffer, "Press any key");
  painter_->DrawStringCenter(0.5, 0.5, str_buffer, strlen(str_buffer));
  painter_->ApplyDrawing();
}