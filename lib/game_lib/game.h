#include "../info_structures/game_info.h"
#include "../info_structures/player_info.h"
#include "game_objects/projectile_game_object.h"
#include <unordered_map>
#include <unordered_set>

class Game {
public:
  void Init(const std::vector<PlayerInfo>& players);
  GameInfo GetInfo();
  bool MovePlayer(int player_id, Direction direction);
  int GetUpdateWaitTime() const;
  void ShootProjectile(int player_id);
  void UpdateGame(int update_num = 1);
  void KillPlayer(int player_id);
  bool IsEnded();

private:
  std::shared_ptr<GameMap> map_;
  std::unordered_map<int, std::shared_ptr<PlayerObject> > players_;
  std::unordered_set<std::shared_ptr<ProjectileObject> > projectiles_;

  GameInfo game_info_;

  int initial_health_;
  int hit_value_;
  int recharge_duration_;
  int stay_health_drop_;
  int movement_health_drop_;
  double mining_time_;
  double step_standard_delay_;
  double moratory_duration_;
  bool is_ended_{false};
};