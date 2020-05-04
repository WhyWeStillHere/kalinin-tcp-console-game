#include "game_settings.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

bool compare_strings(const char* first, const char* second) {
  return (strcmp(first, second) == 0) && (strlen(first) == strlen(second));
}

GameSettings::GameSettings() {
  mutex_ = reinterpret_cast<pthread_mutex_t*>(malloc(sizeof(pthread_mutex_t)));
  pthread_mutex_init(mutex_, NULL);
}

GameSettings::~GameSettings() {
  pthread_mutex_destroy(mutex_);
  free(mutex_);
}

void GameSettings::LoadSettings(const char *file_path) {
  FILE* settings = fopen(file_path, "r");
  if (settings == NULL) {
    throw std::runtime_error("No setting file");
  }
  char buffer[4096];
  fscanf(settings, "%s", buffer);
  if (!compare_strings(buffer, "Map")) {
    throw std::runtime_error("No map in settings");
  }
  int ret = fscanf(settings, "%dx%d", &map_height_, &map_width_);
  if (ret != 2) {
    throw std::runtime_error("Incorrect map size");
  }
  if (map_height_ <= 5 || map_width_ <= 5) {
    throw std::runtime_error("To small map");
  }
  map_height_ += 2;
  map_width_ += 2;
  int free_space = 0;
  map_.resize(map_height_, std::vector<char> (map_width_, ' '));
  for (int i = 0; i < map_height_; ++i) {
    for (int j = 0; j < map_width_; ++j) {
      do {
        ret = fscanf(settings, "%c", &map_[i][j]);
        if (ret <= 0) {
          throw std::runtime_error("Incorrect file size");
        }
      } while (map_[i][j] == '\n');
      if (map_[i][j] != ' ' && map_[i][j] != '#') {
        throw std::runtime_error("Incorrect map symbol");
      }
      if (map_[i][j] == ' ') {
        ++free_space;
      }
    }
  }
  if (free_space < 20) {
    throw std::runtime_error("Too little free space");
  }
  while(true) {
    ret = fscanf(settings, "%s", buffer);
    if (ret <= 0) {
      return;
    }
    if (compare_strings(buffer, "items") ||
        compare_strings(buffer, "items:")) {
      break;
    }
    if (compare_strings(buffer, "initial_health")) {
      ret = fscanf(settings, "%s %d", buffer, &initial_health_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect initial_health");
      }
      continue;
    }
    if (compare_strings(buffer, "hit_value")) {
      ret = fscanf(settings, "%s %d", buffer, &hit_value_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect hit_value");
      }
      continue;
    }
    if (compare_strings(buffer, "recharge_duration")) {
      ret = fscanf(settings, "%s %d", buffer, &recharge_duration_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect recharge_duration");
      }
      continue;
    }
    if (compare_strings(buffer, "mining_time")) {
      ret = fscanf(settings, "%s %lf", buffer, &mining_time_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect mining_time");
      }
      continue;
    }
    if (compare_strings(buffer, "stay_health_drop")) {
      ret = fscanf(settings, "%s %d", buffer, &stay_health_drop_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect stay_health_drop");
      }
      continue;
    }
    if (compare_strings(buffer, "movement_health_drop")) {
      ret = fscanf(settings, "%s %d", buffer, &movement_health_drop_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect movement_health_drop");
      }
      continue;
    }
    if (compare_strings(buffer, "step_standard_delay")) {
      ret = fscanf(settings, "%s %lf", buffer, &step_standard_delay_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect step_standard_delay_");
      }
      continue;
    }
    if (compare_strings(buffer, "moratory_duration")) {
      ret = fscanf(settings, "%s %lf", buffer, &moratory_duration_);
      if (ret < 2 || strlen(buffer) != 1) {
        throw std::runtime_error("Incorrect moratory_duration");
      }
      continue;
    }

    throw std::runtime_error("Unknown parameter");
  }

  while (true) {
    MapItem item;
    ret = fscanf(settings, "%d %d %d", &item.x, &item.y, &item.value);
    if (ret <= 0) {
      break;
    }
    if (ret < 3) {
      throw std::runtime_error("Incorrect item info");
    }
    if (item.x <= 0 || item.x >= map_width_ || item.y <= 0 ||
        item.y >= map_height_ || map_[item.y][item.x] != ' ') {
      throw std::runtime_error("Incorrect item position");
    }
    items_.push_back(item);
  }
}

void GameSettings::Lock() {
  pthread_mutex_lock(mutex_);
}

void GameSettings::Unlock() {
  pthread_mutex_unlock(mutex_);
}

GameSettings* GetCurrentSettings() {
  return &settings;
}