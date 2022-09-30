
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
    auto computeCollision = [](Box* target, Box* body) -> std::optional<glm::vec3>
    {
      glm::vec3 miScale = target->getScale() / 2.0f;
      glm::mat4 bodyLocal_2_TargetLocal = glm::inverse(target->worldToLocal()) * body->worldToLocal();

      float distMin = FLT_MAX;
      bool collision = false;
      glm::vec3 position;

      const auto vertices = body->getVertices();
      for (const auto& vertex : vertices)
      {
        glm::vec3 pos = bodyLocal_2_TargetLocal * glm::vec4(vertex.position, 1.0);
        if (glm::abs(pos.x) < miScale.x &&
            glm::abs(pos.y) < miScale.y &&
            glm::abs(pos.z) < miScale.z)
        {
          collision = true;

          // TODO: better position selection
          float dist = glm::distance(pos, glm::vec3(0.0));
          if (dist < distMin)
          {
            distMin = dist;
            position = target->worldToLocal() * glm::vec4(pos, 1.0);
          }
        }
      }

      if (!collision)
      {
        return std::nullopt;
      }

      return position;
    };

    auto res1 = computeCollision(body1, body2);
    auto res2 = computeCollision(body2, body1);

    if (!res1 && !res2) // No Collision
    {
      return std::nullopt;
    }
    else if (!res1) // Body1 inside Body2
    {
      return std::make_pair(
        (glm::vec3) (body1->worldToLocal() * glm::vec4(glm::vec3(0), 1.0)),
        *res2);
    }
    else if (!res2) // Body2 inside Body1
    {
      return std::make_pair(
        *res1,
        (glm::vec3) (body2->worldToLocal() * glm::vec4(glm::vec3(0), 1.0)));
    }
    else // Classic Collision
    {
      return std::make_pair(*res1, *res2);
    }
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
