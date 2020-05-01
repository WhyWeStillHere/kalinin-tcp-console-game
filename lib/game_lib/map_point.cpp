#include "map_point.h"
#include <functional>

MapPoint::MapPoint(int x, int y)
    : x(x), y(y) {
}


bool operator==(const MapPoint& lhs, const MapPoint& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

std::size_t MapPointHasher::operator()(const MapPoint& map_point) const
{
  return std::hash<int>()(map_point.x) ^ std::hash<int>()(map_point.y);
}

MapPoint GetNewPosition(MapPoint old_position, Direction direction) {
  switch (direction) {
  case UP:
    return {old_position.x, old_position.y - 1};
  case DOWN:
    return {old_position.x, old_position.y + 1};
  case LEFT:
    return {old_position.x - 1, old_position.y};
  case RIGHT:
    return {old_position.x + 1, old_position.y};
  default:
    return old_position;
  }
}