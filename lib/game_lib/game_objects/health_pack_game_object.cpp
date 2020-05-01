#include "health_pack_game_object.h"

HealthPack::HealthPack() : GameObject(HEALTH_PACK) {
}

HealthPack::HealthPack(const int value) :
    GameObject(HEALTH_PACK),
    health_value_(value) {
}