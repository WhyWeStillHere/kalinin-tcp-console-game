#include "projectile_game_object.h"
#include <unordered_map>
#include <queue>

ProjectileObject::ProjectileObject() : GameObject(PROJECTILE) {
}

ProjectileObject::ProjectileObject(std::weak_ptr<GameMap> map,
                                   MapPoint init_position,
                                   int damage_value)
    : GameObject(PROJECTILE),
      position_(init_position),
      map_(std::move(map)),
      damage_value_(damage_value) {
  FindTarget();
}

bool ProjectileObject::IsTargetAvailable() {
  if (!target_) {
    return false;
  } else {
    return target_->IsAlive();
  }
}

Direction ProjectileObject::TargetDirection() {
  MapPoint start_position = position_;
  MapPoint end_position = target_->position_;
  auto map = map_.lock();
  int map_width = map->GetWidth();
  int map_height = map->GetHeight();

  std::unordered_map<MapPoint, int, MapPointHasher> distance;
  std::unordered_map<MapPoint, MapPoint, MapPointHasher> previous_point;
  std::queue<MapPoint> bfs_queue;
  bfs_queue.push(start_position);
  distance[start_position] = 1;
  while(!bfs_queue.empty() && distance[end_position] == 0) {
    MapPoint current_point = bfs_queue.front();
    int current_distance = distance[current_point];
    bfs_queue.pop();
    if (current_point.x + 1 < map_width) {
      if (map->GetObject({current_point.x + 1, current_point.y})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x + 1, current_point.y}];
        if (distance_to_point == 0) {
          distance[{current_point.x + 1, current_point.y}] = current_distance + 1;
          previous_point[{current_point.x + 1, current_point.y}] = current_point;
          bfs_queue.push({current_point.x + 1, current_point.y});
        }
      }
    }
    if (current_point.x - 1 >= 0) {
      if (map->GetObject({current_point.x - 1, current_point.y})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x - 1, current_point.y}];
        if (distance_to_point == 0) {
          distance[{current_point.x - 1, current_point.y}] = current_distance + 1;
          previous_point[{current_point.x - 1, current_point.y}] = current_point;
          bfs_queue.push({current_point.x - 1, current_point.y});
        }
      }
    }
    if (current_point.y + 1 < map_height) {
      if (map->GetObject({current_point.x, current_point.y + 1})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x, current_point.y + 1}];
        if (distance_to_point == 0) {
          distance[{current_point.x, current_point.y + 1}] = current_distance + 1;
          previous_point[{current_point.x, current_point.y + 1}] = current_point;
          bfs_queue.push({current_point.x, current_point.y + 1});
        }
      }
    }
    if (current_point.y - 1 >= 0) {
      if (map->GetObject({current_point.x, current_point.y - 1})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x, current_point.y - 1}];
        if (distance_to_point == 0) {
          distance[{current_point.x, current_point.y - 1}] = current_distance + 1;
          previous_point[{current_point.x, current_point.y - 1}] = current_point;
          bfs_queue.push({current_point.x, current_point.y - 1});
        }
      }
    }
  }
  if (distance[end_position] == 0) {
    return UNKNOWN_DIRECTION;
  }

  MapPoint current_position = end_position;
  while (true) {
    MapPoint prev_position = previous_point[current_position];
    if (prev_position == start_position) {
      if (current_position.x > start_position.x) {
        return RIGHT;
      }
      if (current_position.x < start_position.x) {
        return LEFT;
      }
      if (current_position.y > start_position.y) {
        return DOWN;
      }
      if (current_position.y < start_position.y) {
        return UP;
      }
    }
    current_position = prev_position;
  }
}

void ProjectileObject::FindTarget() {
  MapPoint start_position = position_;
  auto map = map_.lock();
  int map_width = map->GetWidth();
  int map_height = map->GetHeight();

  std::unordered_map<MapPoint, int, MapPointHasher> distance;
  std::queue<MapPoint> bfs_queue;
  bfs_queue.push(start_position);
  distance[start_position] = 1;
  while(!bfs_queue.empty()) {
    MapPoint current_point = bfs_queue.front();
    int current_distance = distance[current_point];
    bfs_queue.pop();
    if (current_point.x + 1 < map_width) {
      if (map->GetObject({current_point.x + 1, current_point.y})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x + 1, current_point.y}];
        if (distance_to_point == 0) {
          if (map->GetObject({current_point.x + 1, current_point.y})
                  ->type_ == PLAYER) {
            target_ = std::static_pointer_cast<PlayerObject>(
                map->GetObject({current_point.x + 1, current_point.y}));
            return;
          }
          distance[{current_point.x + 1, current_point.y}] = current_distance + 1;
          bfs_queue.push({current_point.x + 1, current_point.y});
        }
      }
    }
    if (current_point.x - 1 >= 0) {
      if (map->GetObject({current_point.x - 1, current_point.y})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x - 1, current_point.y}];
        if (distance_to_point == 0) {

          if (map->GetObject({current_point.x - 1, current_point.y})
                  ->type_ == PLAYER) {
            target_ = std::static_pointer_cast<PlayerObject>(
                map->GetObject({current_point.x - 1, current_point.y}));
            return;
          }

          distance[{current_point.x - 1, current_point.y}] = current_distance + 1;
          bfs_queue.push({current_point.x - 1, current_point.y});
        }
      }
    }
    if (current_point.y + 1 < map_height) {
      if (map->GetObject({current_point.x, current_point.y + 1})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x, current_point.y + 1}];
        if (distance_to_point == 0) {
          if (map->GetObject({current_point.x, current_point.y + 1})
                  ->type_ == PLAYER) {
            target_ = std::static_pointer_cast<PlayerObject>(
                map->GetObject({current_point.x, current_point.y + 1}));
            return;
          }

          distance[{current_point.x, current_point.y + 1}] = current_distance + 1;
          bfs_queue.push({current_point.x, current_point.y + 1});
        }
      }
    }
    if (current_point.y - 1 >= 0) {
      if (map->GetObject({current_point.x, current_point.y - 1})
              ->type_ != WALL) {
        int distance_to_point = distance[{current_point.x, current_point.y - 1}];
        if (distance_to_point == 0) {
          if (map->GetObject({current_point.x, current_point.y - 1})
                  ->type_ == PLAYER) {
            target_ = std::static_pointer_cast<PlayerObject>(
                map->GetObject({current_point.x, current_point.y - 1}));
            return;
          }

          distance[{current_point.x, current_point.y - 1}] = current_distance + 1;
          bfs_queue.push({current_point.x, current_point.y - 1});
        }
      }
    }
  }
}

MapPoint ProjectileObject::SpawnPosition() {
  if (!IsTargetAvailable()) {
    return {-1, -1};
  }
  Direction move_direction = TargetDirection();
  if (move_direction == UNKNOWN_DIRECTION) {
    return {-1, -1};
  }
  MapPoint new_position(GetNewPosition(position_, move_direction));
  position_ = new_position;
  return position_;
}

bool ProjectileObject::Move() {
  if (!IsTargetAvailable()) {
    return false;
  }
  --movements_left;
  if (movements_left <= 0) {
    return false;
  }
  Direction move_direction = TargetDirection();
  if (move_direction == UNKNOWN_DIRECTION) {
    return false;
  }
  MapPoint new_position(GetNewPosition(position_, move_direction));
  auto map = map_.lock();
  if (map->GetObject(new_position)->type_ == PLAYER) {
    std::static_pointer_cast<PlayerObject>(map->GetObject(new_position))->Hit(damage_value_);
    return false;
  }
  if (map->GetObject(new_position)->type_ == PROJECTILE) {
    return false;
  }
  map->MoveObject(position_, new_position);
  position_ = new_position;
  return true;
}

MapPoint ProjectileObject::GetPosition() const {
  return position_;
}

int ProjectileObject::GetDamageValue() const {
  return damage_value_;
}