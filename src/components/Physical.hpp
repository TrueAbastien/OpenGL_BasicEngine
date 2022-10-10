#pragma once

#include "Meshable.hpp"

class Physical : public Meshable
{
protected:
  Physical();

public:
  virtual ~Physical();

protected:
  virtual void beforeInitialize(Renderer* renderer) override;

public:
  virtual CurrentTargetCollisions computeCollision(CollisionManager* colMan) = 0;

private:
  //Renderer* m_rendererRef;
};
