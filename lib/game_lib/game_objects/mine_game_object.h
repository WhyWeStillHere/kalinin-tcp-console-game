#include "game_object.h"

class MineObject: public GameObject {
public:
  MineObject();

private:
  const int damage_value_ = 20;
};