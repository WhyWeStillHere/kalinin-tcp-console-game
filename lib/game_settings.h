#include <pthread.h>
#include <vector>

struct MapItem {
  int x, y;
  int value;
};

class GameSettings {
public:
  GameSettings();
  ~GameSettings();

  void Lock();
  void Unlock();
  void LoadSettings(const char* file_path = "./server_settings");

  int map_height_, map_width_;
  std::vector<std::vector<char> > map_;
  int initial_health_ = 500;
  int hit_value_ = 50;
  int recharge_duration_ = 3;
  int stay_health_drop_ = 1;
  int movement_health_drop_ = 4;
  double mining_time_ = 6;
  double step_standard_delay_ = 0.1;
  double moratory_duration_ = 5;
  std::vector<MapItem> items_;

private:
  pthread_mutex_t* mutex_;
};

static GameSettings settings;

GameSettings* GetCurrentSettings() {
  return &settings;
}