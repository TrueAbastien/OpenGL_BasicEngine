#pragma once

#include "Physical.hpp"

class Box : public Physical
{
public:
  Box(glm::vec3 scale);

  glm::vec3 getScale() const;

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

protected:
  glm::vec3 m_scale;
};
