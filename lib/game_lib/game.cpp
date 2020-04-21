#include "game.h"

void Game::Init(const std::vector<PlayerInfo>& players) {
  map_.Init();
  GameSettings* game_settings = GetCurrentSettings();
  game_settings->Lock();
  initial_health_ = game_settings->initial_health_;
  hit_value_ = game_settings->hit_value_;
  recharge_duration_ = game_settings->recharge_duration_;
  stay_health_drop_ = game_settings->stay_health_drop_;
  movement_health_drop_ = game_settings->movement_health_drop_;
  mining_time_ = game_settings->mining_time_;
  step_standard_delay_ = game_settings->step_standard_delay_;
  moratory_duration_ = game_settings->moratory_duration_;
  game_settings->Unlock();
  for (const auto& player: players) {
    auto player_ptr = std::make_shared<PlayerObject>(player.id, initial_health_);
    map_.PlacePlayer(player_ptr);
    players_[player.id] = player_ptr;
  }
}

GameInfo Game::GetInfo() {
  GameInfo info;
  info.map = map_.GetInfo();
  for (const auto& player: players_) {
    info.players_info.emplace_back(player.second->GetInfo());
  }
  return std::move(info);
}

bool Game::MovePlayer(int player_id, Direction direction) {
  MapPoint new_position;
  std::shared_ptr<PlayerObject> player(players_[player_id]);
  MapPoint current_position = player->position_;
  switch (direction) {
    case UP:
      new_position = {current_position.x, current_position.y - 1};
      break;
    case DOWN:
      new_position = {current_position.x, current_position.y + 1};
      break;
    case LEFT:
      new_position = {current_position.x - 1, current_position.y};
      break;
    case RIGHT:
      new_position = {current_position.x + 1, current_position.y};
      break;
  }
  if (!map_.IsAvailable(new_position)) {
    return false;
  }
  map_.MovePlayer(current_position, new_position);
  player->position_ = new_position;
  return true;
}