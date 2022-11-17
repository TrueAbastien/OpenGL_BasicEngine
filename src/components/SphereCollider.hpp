#pragma once

#include "Physical.hpp"

#include "builders/WFSphereBuilder.hpp"

class SphereCollider final : public Physical, public WFSphereBuilder
{
public:
  SphereCollider(const std::shared_ptr<Meshable>& target);

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
