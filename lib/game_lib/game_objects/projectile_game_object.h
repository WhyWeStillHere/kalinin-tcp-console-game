#include "../../direction.h"
#include "../game_map.h"
#include "../map_point.h"
#include "game_object.h"
#include <memory>

class ProjectileObject: public GameObject {
public:
  ProjectileObject();
  ProjectileObject(std::weak_ptr<GameMap> map,
                   MapPoint init_position,
                   int damage_value);

  Direction TargetDirection();
  bool IsTargetAvailable();
  bool Move();
  MapPoint GetPosition() const;
  MapPoint SpawnPosition();
  int GetDamageValue() const;

private:
  void FindTarget();

  int damage_value_ = 20;
  int movements_left = 10;
  MapPoint position_;
  std::shared_ptr<PlayerObject> target_;
  std::weak_ptr<GameMap> map_;
};