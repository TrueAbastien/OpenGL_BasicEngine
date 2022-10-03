
#include "CollisionManager.hpp"


// ------------------------------------------------------------------------------------------------
namespace CollisionUtils
{
}

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
void CollisionManager::clearCache()
{
  m_cachedResults.clear();
}
