#include "Physical.hpp"

#include "Renderer.hpp"
#include "CollisionManager.hpp"

// ------------------------------------------------------------------------------------------------
Physical::Physical()
  : Meshable(), m_body(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
Physical::~Physical()
{
  // TODO: remove Physical on destruction
}

// ------------------------------------------------------------------------------------------------
RigidBody* Physical::getRigidBody() const
{
  return m_body;
}

// ------------------------------------------------------------------------------------------------
bool Physical::hasBody() const
{
  return (m_body != nullptr);
}

// ------------------------------------------------------------------------------------------------
void Physical::beforeInitialize(Renderer* renderer)
{
  renderer->getCollisionManager()->addPhysical(this);
}

// ------------------------------------------------------------------------------------------------
void Physical::setRigidBody(RigidBody* body)
{
  m_body = body;
}
