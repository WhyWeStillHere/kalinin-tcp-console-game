#pragma once

enum GameObjectType {
  PLAYER,
  WALL,
  HEALTH_PACK,
  MINE,
  PROJECTILE,
  VOID
};

class GameObject {
public:
  GameObject() = delete;
  virtual ~GameObject() {}
  explicit GameObject(GameObjectType type);

  const GameObjectType type_;
};
