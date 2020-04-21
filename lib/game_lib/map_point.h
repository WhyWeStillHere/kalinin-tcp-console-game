//
// Created by whywestillhere on 19.04.2020.
//

#ifndef CONSOLE_GAME_MAP_POINT_H
#define CONSOLE_GAME_MAP_POINT_H

#pragma once

struct MapPoint {
  MapPoint() = default;
  MapPoint(int x, int y) : x(x), y(y) {}

  int x, y;
};

#endif // CONSOLE_GAME_MAP_POINT_H
