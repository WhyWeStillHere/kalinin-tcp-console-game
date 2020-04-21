#include "../info_structures/game_info.h"
#include "../info_structures/player_info.h"
#include "game_map.h"
#include <unordered_map>

class Game {
public:
  void Init(const std::vector<PlayerInfo>& players);
  GameInfo GetInfo();
  bool MovePlayer(int player_id, Direction direction);

private:
  void UpdateMapInfo();
  void UpdatePlayerInfo();
  void UpdateGameInfo();
  GameMap map_;
  std::unordered_map<int, std::shared_ptr<PlayerObject> > players_;

  GameInfo game_info_;

  int initial_health_;
  int hit_value_;
  int recharge_duration_;
  int stay_health_drop_;
  int movement_health_drop_;
  double mining_time_;
  double step_standard_delay_;
  double moratory_duration_;
};