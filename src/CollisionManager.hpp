
#pragma once

#include "Component.hpp"

#include <optional>
#include <utility>
#include <tuple>


// Data Response ----------------------------------------------------------------------------------
// - First pair value is closest Body2 point (from Body1) in Body1
// - Second pair value is closest Body1 point (from Body2) in Body2
using CollisionInternalResult = std::pair<glm::vec3, glm::vec3>;
// - Optional on Collision (inexistant means no Collision
using CollisionResult = std::optional<CollisionInternalResult>;

// ------------------------------------------------------------------------------------------------
namespace CollisionUtils
{
  template <typename T>
  concept PhysicalDerived = std::derived_from<T, Physical>;

  // Utility Methods ------------------------------------------------------------------------------
  inline CollisionResult swap(const CollisionResult& res)
  {
    if (!res.has_value())
      return std::nullopt;

    return std::make_pair(res->second, res->first);
  }

  // Generic Definition ---------------------------------------------------------------------------
  template <PhysicalDerived T>
  inline int priority()
  {
    return -1;
  }

  template <PhysicalDerived T1, PhysicalDerived T2>
  inline CollisionResult internalCompute(T1* body1, T2* body2)
  {
    return std::nullopt;
  }

  template <PhysicalDerived T1, PhysicalDerived T2>
  inline CollisionResult compute(T1* body1, T2* body2)
  {
    return (priority<T1>() > priority<T2>())
      ? swap(internalCompute(body2, body1))
      : internalCompute(body1, body2);
  }

  // Variadic Casting -----------------------------------------------------------------------------
  template <PhysicalDerived TTarget>
  inline CollisionResult physicalCastCompute(TTarget* target, Physical* body)
  {
    return std::nullopt;
  }

  template <PhysicalDerived TTarget, PhysicalDerived TBody>
  inline CollisionResult physicalCastCompute(TTarget* target, Physical* body)
  {
    auto result = dynamic_cast<TBody*>(body);
    return (result == nullptr) ? std::nullopt
      : compute<TTarget, TBody>(target, result);
  }

  template <PhysicalDerived TTarget, PhysicalDerived TBody, PhysicalDerived ...Types>
  inline CollisionResult physicalCastCompute(TTarget* target, Physical* body)
  {
    auto result = dynamic_cast<TBody*>(body);
    return (result == nullptr)
      ? physicalCastCompute<TTarget, Types...>(target, body)
      : compute<TTarget, TBody>(target, result);
  }

  template <PhysicalDerived T>
  inline CollisionResult concreteCompute(T* target, Physical* body)
  {
    return physicalCastCompute<T,

      // List of all Physicals
      Box
      // TO COMPLETE

    >(target, body);
  }

  // Box Definitions ------------------------------------------------------------------------------
  template <>
  inline int priority<Box>()
  {
    return 0;
  }

  template <>
  inline CollisionResult internalCompute<Box, Box>(Box* body1, Box* body2)
  {
    auto isColliding = [](Box* target, Box* body) -> bool
    {
      glm::vec3 miScale = target->getScale() * 0.5f;
      glm::mat4 bodyLocal_2_TargetLocal = glm::inverse(target->worldToLocal()) * body->worldToLocal();

      const auto vertices = body->getVertices();
      for (const auto& vertex : vertices)
      {
        glm::vec3 pos = bodyLocal_2_TargetLocal * glm::vec4(vertex.position, 1.0);
        if (glm::abs(pos.x) < miScale.x &&
            glm::abs(pos.y) < miScale.y &&
            glm::abs(pos.z) < miScale.z)
        {
          return true;
        }
      }

      return false;
    };

    auto closestPoint = [](Box* target, Box* body) -> glm::vec3
    {
      glm::mat4 bodyLocal_2_TargetLocal = glm::inverse(target->worldToLocal()) * body->worldToLocal();

      glm::vec3 targetOrigin_inBody = glm::inverse(bodyLocal_2_TargetLocal) * glm::vec4(0.0, 0.0, 0.0, 1.0);
      glm::vec3 miScale = body->getScale() * 0.5f;

      float factor = std::min(
        {
          miScale.x / targetOrigin_inBody.x,
          miScale.y / targetOrigin_inBody.y,
          miScale.z / targetOrigin_inBody.z
        });

      if (factor >= 1.0f)
      {
        return glm::vec3(0.0);
      }

      return bodyLocal_2_TargetLocal * glm::vec4(targetOrigin_inBody * factor, 1.0);
    };

    if (!isColliding(body1, body2) && !isColliding(body2, body1))
    {
      return std::nullopt;
    }

    return std::make_pair(
      closestPoint(body1, body2),
      closestPoint(body2, body1)
    );
  }
}

// ------------------------------------------------------------------------------------------------
class CollisionManager final
{
public:
  using Result = std::map<Physical*, CollisionInternalResult>;

public:
  CollisionManager();

  bool addPhysical(Physical* physical);
  bool removePhysical(Physical* physical);

  template <CollisionUtils::PhysicalDerived T>
  Result computeCollision(T* target);

  void clearCache();

private:
  std::vector<Physical*> m_colliders;
  std::map<Physical*, Result> m_cachedResults;
};

// ------------------------------------------------------------------------------------------------
template<CollisionUtils::PhysicalDerived T>
inline CollisionManager::Result CollisionManager::computeCollision(T* target)
{
  Result result;

  for (Physical* physical : m_colliders)
  {
    if (physical == target)
    {
      continue;
    }

    if (m_cachedResults[physical].contains(target))
    {
      result[physical] = m_cachedResults[physical][target];
      continue;
    }

    CollisionResult res = CollisionUtils::concreteCompute(target, physical);
    if (!res.has_value())
    {
      continue;
    }

    result[physical] = *res;
  }

  // Cache results to prevent recomputation
  m_cachedResults[target] = result;

  return result;
}
