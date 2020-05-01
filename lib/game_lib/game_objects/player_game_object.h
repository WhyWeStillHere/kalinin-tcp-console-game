#include "../../info_structures/player_object_info.h"
#include "../map_point.h"
#include "game_object.h"

class PlayerObject: public GameObject {
public:
  PlayerObject();
  PlayerObject(int id, int health_value);

  PlayerObjectInfo GetInfo() const;
  bool IsMoved() const;
  bool IsAlive() const;
  bool Hit(int hit_damage);
  void SetMoveFl();
  void ResetMoveFl();
  void Kill();

public:
  MapPoint position_;
  int health_value_ = 20;
  const int id_ = -1;
  bool is_moved = false;
};
