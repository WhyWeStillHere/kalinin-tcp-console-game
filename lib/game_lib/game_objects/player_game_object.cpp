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
