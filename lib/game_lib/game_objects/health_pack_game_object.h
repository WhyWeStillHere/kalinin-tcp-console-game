#include "game_object.h"

class HealthPackObject : public GameObject {
public:
  HealthPackObject();
  HealthPackObject(int value);

  int GetValue();

private:
  const int health_value_ = 20;
};