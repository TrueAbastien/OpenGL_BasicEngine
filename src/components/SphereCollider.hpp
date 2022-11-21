#pragma once

#include "Physical.hpp"
#include "TexturedMesh.hpp"

#include "builders/WFSphereBuilder.hpp"

class SphereCollider final : public Physical, public WFSphereBuilder
{
public:
  SphereCollider(const std::shared_ptr<Meshable>& target);
  SphereCollider(const std::shared_ptr<TexturedMesh>& mesh);

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
