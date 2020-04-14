#pragma once

enum GameObjectType {
  PLAYER,
  WALL,
  HEALTH_PACK,
  MINE,
  VOID
};

class GameObject {
public:
  GameObject() = delete;
  explicit GameObject(GameObjectType type);

  const GameObjectType type_;
};
