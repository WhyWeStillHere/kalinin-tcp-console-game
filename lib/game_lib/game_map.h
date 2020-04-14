#include "game_objects/game_object.h"
#include "game_objects/void_game_object.h"
#include "game_objects/wall_game_object.h"
#include "game_objects/player_game_object.h"
#include "../game_settings.h"
#include <vector>
#include <memory>

struct MapPoint {
  MapPoint(int x, int y) : x(x), y(y) {}

  int x, y;
};

class GameMap {
public:
  GameMap();

  void Init();

  int GetWidth() {
    return width_;
  }

  int GetHeight() {
    return height_;
  }

  void PlacePlayer(std::shared_ptr<PlayerObject> player);

private:
  MapPoint GetRandomValidPoint();
  std::vector< std::vector<std::shared_ptr<GameObject> > > map_;
  int width_, height_;

  std::shared_ptr<VoidObject> void_obj_;
  std::shared_ptr<WallObject> wall_obj_;
};
