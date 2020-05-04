#include "health_pack_game_object.h"

HealthPackObject::HealthPackObject() : GameObject(HEALTH_PACK) {
}

HealthPackObject::HealthPackObject(const int value) :
    GameObject(HEALTH_PACK),
    health_value_(value) {
}

int HealthPackObject::GetValue() {
  return health_value_;
}