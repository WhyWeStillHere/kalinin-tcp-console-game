//
// Created by whywestillhere on 19.04.2020.
//

#ifndef CONSOLE_GAME_MAP_POINT_H
#define CONSOLE_GAME_MAP_POINT_H

#pragma once

#include "../direction.h"
#include <cstdio>

struct MapPoint {
  MapPoint() = default;
  MapPoint(int x, int y);

  int x, y;
};

bool operator==(const MapPoint& lhs, const MapPoint& rhs);

struct MapPointHasher
{
  std::size_t operator()(const MapPoint& k) const;
};

MapPoint GetNewPosition(MapPoint old_position, Direction direction);

#endif // CONSOLE_GAME_MAP_POINT_H
