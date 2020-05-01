#include "player_game_object.h"

PlayerObject::PlayerObject() : GameObject(PLAYER) {
}

PlayerObject::PlayerObject(const int id, const int health_value)
  : GameObject(PLAYER), id_(id), health_value_(health_value) {
}

PlayerObjectInfo PlayerObject::GetInfo() const {
  PlayerObjectInfo info;
  info.id = id_;
  info.health_value = health_value_;
  info.x_coord = position_.x;
  info.y_coord = position_.y;
  return info;
}

bool PlayerObject::Hit(int hit_damage) {
  health_value_ = std::max(health_value_ - hit_damage, 0);
  return health_value_ == 0;
}

bool PlayerObject::IsMoved() const {
  return is_moved;
}

void PlayerObject::SetMoveFl() {
  is_moved = true;
}

void PlayerObject::ResetMoveFl() {
  is_moved = false;
}

bool PlayerObject::IsAlive() const {
  return health_value_ > 0;
}

void PlayerObject::Kill() {
  health_value_ = 0;
}