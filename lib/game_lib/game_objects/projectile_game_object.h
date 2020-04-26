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

private:
  void FindTarget();

  int damage_value_ = 20;
  MapPoint position_;
  std::shared_ptr<PlayerObject> target_;
  std::weak_ptr<GameMap> map_;
};