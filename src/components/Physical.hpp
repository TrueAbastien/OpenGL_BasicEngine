#pragma once

#include "Meshable.hpp"

class Physical : public Meshable
{
public:
  friend RigidBody;

protected:
  Physical(const std::shared_ptr<Meshable>& target);

public:
  virtual ~Physical();

  RigidBody* getRigidBody() const;
  bool hasBody() const;

protected:
  virtual void beforeInitialize(Renderer* renderer) override;

  void setRigidBody(RigidBody* body);

public:
  virtual CurrentTargetCollisions computeCollision(CollisionManager* colMan) = 0;

private:
  RigidBody* m_body;
};
