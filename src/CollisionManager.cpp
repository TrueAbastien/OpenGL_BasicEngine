#include "CollisionManager.hpp"

// ------------------------------------------------------------------------------------------------
CollisionManager::CollisionManager()
  : m_colliders(std::vector<Physical*>(0)),
  m_cachedResults{ }
{
}

// ------------------------------------------------------------------------------------------------
bool CollisionManager::addPhysical(Physical* physical)
{
  if (physical == nullptr)
  {
    return false;
  }

  auto it = std::find(m_colliders.begin(), m_colliders.end(), physical);
  if (it != m_colliders.end())
  {
    return false;
  }

  m_colliders.push_back(physical);

  return true;
}

// ------------------------------------------------------------------------------------------------
bool CollisionManager::removePhysical(Physical* physical)
{
  if (physical == nullptr)
  {
    return false;
  }

  auto it = std::find(m_colliders.begin(), m_colliders.end(), physical);
  if (it == m_colliders.end())
  {
    return false;
  }

  m_colliders.erase(it);

  return true;
}

// ------------------------------------------------------------------------------------------------
void CollisionManager::clearAll()
{
  m_colliders.clear();
  m_cachedResults.clear();
}

// ------------------------------------------------------------------------------------------------
CollisionManager::ResultMap CollisionManager::computeAllCollisions()
{
  m_cachedResults.clear();

  for (const auto& physical : m_colliders)
  {
    physical->computeCollision(this);
  }

  return m_cachedResults;
}
