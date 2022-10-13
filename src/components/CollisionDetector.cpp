#include "CollisionDetector.hpp"

// ------------------------------------------------------------------------------------------------
CollisionDetector::CollisionDetector(CollisionManager* manager)
  : m_manager(manager)
{
}

// ------------------------------------------------------------------------------------------------
void CollisionDetector::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  auto collisionsMap = m_manager->computeAllCollisions();

  for (auto& collisions : collisionsMap)
  {
    // TODO
  }
}
