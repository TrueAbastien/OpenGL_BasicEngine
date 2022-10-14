#pragma once

#include "Physical.hpp"

#include "builders/WFBoxBuilder.hpp"

class BoxCollider final : public Physical, public WFBoxBuilder
{
public:
  BoxCollider(const std::shared_ptr<Meshable>& target);

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
