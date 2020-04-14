#include "../../info_structures/player_object_info.h"
#include "game_object.h"

class PlayerObject: public GameObject {
public:
  PlayerObject();
  PlayerObject(int id, int health_value);

  PlayerObjectInfo GetInfo() const;

private:
  const int health_value_ = 20;
  const int id_ = -1;
};
