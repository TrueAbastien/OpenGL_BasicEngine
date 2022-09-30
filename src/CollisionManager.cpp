
#include "CollisionManager.hpp"


// ------------------------------------------------------------------------------------------------
namespace CollisionUtils
{
  // Box Definitions ------------------------------------------------------------------------------
  template <>
  inline int priority<Box>()
  {
    return 0;
  }

  template <>
  inline CollisionResult internalCompute<Box, Box>(Box* body1, Box* body2)
  {
    // TODO
    return std::nullopt;
  }
}

// ------------------------------------------------------------------------------------------------
CollisionManager::CollisionManager()
  : m_colliders(std::vector<Physical*>(0))
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
