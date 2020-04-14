#include "game_object.h"

class HealthPack: public GameObject {
public:
  HealthPack();
  HealthPack(int value);

private:
  const int health_value_ = 20;
};