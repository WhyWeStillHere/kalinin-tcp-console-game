#include "game.h"
#include "game_objects/health_pack_game_object.h"

void Game::Init(const std::vector<PlayerInfo>& players) {
  map_ = std::make_shared<GameMap>();
  map_->Init();
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
    map_->PlacePlayer(player_ptr);
    players_[player.id] = player_ptr;
  }
}

GameInfo Game::GetInfo() {
  GameInfo info;
  info.map = map_->GetInfo();
  for (const auto& player: players_) {
    info.players_info.emplace_back(player.second->GetInfo());
  }
  return std::move(info);
}

bool Game::MovePlayer(int player_id, Direction direction) {
  MapPoint new_position;
  std::shared_ptr<PlayerObject> player(players_[player_id]);
  if (!player->IsAlive()) {
    return false;
  }
  MapPoint current_position = player->position_;

  new_position = GetNewPosition(current_position, direction);

  if (!map_->IsAvailable(new_position)) {
    return false;
  }
  std::shared_ptr<GameObject> game_object(map_->GetObject(new_position));
  if (game_object->type_ == PROJECTILE) {
    std::shared_ptr<ProjectileObject> projectile(
        std::static_pointer_cast<ProjectileObject>(game_object));
    player->Hit(projectile->GetDamageValue());
    projectiles_.erase(projectile);
  }
  if (game_object->type_ == HEALTH_PACK) {
    std::shared_ptr<HealthPackObject> health_pack(
        std::static_pointer_cast<HealthPackObject>(game_object));
    player->Hit(-health_pack->GetValue());
  }
  map_->MoveObject(current_position, new_position);
  player->position_ = new_position;
  player->SetMoveFl();
  return true;
}

int Game::GetUpdateWaitTime() const {
  return static_cast<int>(step_standard_delay_ * 1000);
}

void Game::KillPlayer(int player_id) {
  std::shared_ptr<PlayerObject> player(players_[player_id]);
  player->Kill();
}

void Game::UpdateGame(const int update_num) {
  std::vector<int> dead_players;
  int alive_players = 0;
  for (auto& player: players_) {
    if (!player.second->IsAlive()) {
      continue;
    }
    if (player.second->IsMoved()) {
      player.second->Hit(movement_health_drop_);
      player.second->Hit(stay_health_drop_ * (update_num - 1));
    } else {
      player.second->Hit(stay_health_drop_ * update_num);
    }
    player.second->ResetMoveFl();
  }

  std::vector<std::shared_ptr<ProjectileObject> > to_remove;
  for (auto& projectile: projectiles_) {
    if (!projectile->Move()) {
      map_->Clear(projectile->GetPosition());
      to_remove.push_back(projectile);
    }
  }

  for (auto& projectile: to_remove) {
    projectiles_.erase(projectile);
  }

  for (auto& player: players_) {
    if (player.second->IsAlive()) {
      ++alive_players;
    }
  }
  if (alive_players <= 1) {
    is_ended_ = true;
  }
}

void Game::ShootProjectile(int player_id) {
  std::shared_ptr<PlayerObject> player(players_[player_id]);
  auto bullet_ptr = std::make_shared<ProjectileObject>(map_,
                                                       player->position_,
                                                       hit_value_);
  MapPoint spawn_place = bullet_ptr->SpawnPosition();
  if (spawn_place.x == -1 && spawn_place.y == -1 ||
      spawn_place.x == player->position_.x && spawn_place.y == player->position_.y) {
    return;
  }
  std::shared_ptr<GameObject> game_object(map_->GetObject(spawn_place));
  if (game_object->type_ == PROJECTILE) {
    return;
  }
  if (game_object->type_ == PLAYER) {
    std::shared_ptr<PlayerObject> player(
        std::static_pointer_cast<PlayerObject>(game_object));
    player->Hit(bullet_ptr->GetDamageValue());
    return;
  }
  map_->SetObject(spawn_place, bullet_ptr);
  projectiles_.insert(std::move(bullet_ptr));
}

bool Game::IsEnded() {
  return is_ended_;
}