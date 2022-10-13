#pragma once

#include "Physical.hpp"

#include "builders/BoxBuilder.hpp"

class BoxCollider final : public Physical, public BoxBuilder
{
public:
  BoxCollider(const std::shared_ptr<Meshable>& target);

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
