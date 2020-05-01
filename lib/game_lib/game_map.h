#include "../direction.h"
#include "../game_settings.h"
#include "../info_structures/game_map_info.h"
#include "game_objects/game_object.h"
#include "game_objects/player_game_object.h"
#include "game_objects/void_game_object.h"
#include "game_objects/wall_game_object.h"
#include "map_point.h"
#include <memory>
#include <vector>

class GameMap : public std::enable_shared_from_this<GameMap> {
public:
  GameMap();

  void Init();
  GameMapInfo GetInfo();
  std::shared_ptr<GameObject> GetObject(MapPoint position);
  void SetObject(MapPoint position, std::shared_ptr<GameObject> object);

  bool IsAvailable(MapPoint position);
  void MoveObject(MapPoint object_position, MapPoint destination_position);

  void PlacePlayer(std::shared_ptr<PlayerObject> player);
  void Clear(MapPoint position);
  int GetWidth();
  int GetHeight();

private:
  MapPoint GetRandomValidPoint();
  std::vector< std::vector<std::shared_ptr<GameObject> > > map_;
  int width_;
  int height_;

  std::shared_ptr<VoidObject> void_obj_;
  std::shared_ptr<WallObject> wall_obj_;
};
