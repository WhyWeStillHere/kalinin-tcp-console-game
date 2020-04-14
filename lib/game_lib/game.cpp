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
    players_.push_back(player_ptr);
  }
}

void Game::UpdatePlayerInfo() {
  std::vector<PlayerObjectInfo> players_info;
  for (const auto& player: players_) {
    PlayerInfo info;
  }
}