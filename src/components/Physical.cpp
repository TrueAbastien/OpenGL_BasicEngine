#include "Physical.hpp"

#include "Renderer.hpp"
#include "CollisionManager.hpp"

// ------------------------------------------------------------------------------------------------
Physical::Physical()
  : Meshable()//, m_rendererRef(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
Physical::~Physical()
{
  // TODO: remove Physical on destruction
  // 
  //m_rendererRef->getCollisionManager()->removePhysical(this);
}

// ------------------------------------------------------------------------------------------------
void Physical::beforeInitialize(Renderer* renderer)
{
  //m_rendererRef = renderer;

  renderer->getCollisionManager()->addPhysical(this);
}
