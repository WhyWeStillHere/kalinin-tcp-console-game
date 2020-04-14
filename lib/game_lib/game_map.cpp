#include "game_map.h"
#include "game_objects/health_pack_game_object.h"
#include <random>
#include <chrono>

GameMap::GameMap() {
  void_obj_ = std::make_shared<VoidObject>();
  wall_obj_ = std::make_shared<WallObject>();
}

void GameMap::Init() {
  GameSettings* global_settings = GetCurrentSettings();
  std::vector< std::vector<char> > simple_map;
  std::vector<MapItem> items;
  global_settings->Lock();
  width_ = global_settings->map_width_;
  height_ = global_settings->map_height_;
  simple_map = global_settings->map_;
  items = global_settings->items_;
  global_settings->Unlock();
  map_.resize(width_, std::vector< std::shared_ptr<GameObject> >  (height_));
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      if (simple_map[i][j] == '.') {
        map_[j][i] = void_obj_;
      } else {
        map_[j][i] = wall_obj_;
      }
    }
  }
  for (const auto& item: items) {
    map_[item.x][item.y] = std::make_shared<HealthPack>(item.value);
  }
}

MapPoint GameMap::GetRandomValidPoint() {
  unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 g(seed);
  int attempts = width_ * height_ * 10;
  for (int i = 0; i < attempts; ++i) {
    int x = g() % width_;
    int y = g() % height_;
    if (map_[x][y]->type_ == VOID) {
      return {x, y};
    }
  }
  throw std::runtime_error("Not enough space for player");
}

void GameMap::PlacePlayer(std::shared_ptr<PlayerObject> player) {
  MapPoint point = GetRandomValidPoint();
  map_[point.x][point.y] = player;
}
