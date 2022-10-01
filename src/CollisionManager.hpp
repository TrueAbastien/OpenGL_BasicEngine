
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
            position = pos;
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
        glm::vec3(0),
        *res2);
    }
    else if (!res2) // Body2 inside Body1
    {
      return std::make_pair(
        *res1,
        glm::vec3(0));
    }
    else // Classic Collision
    {
      return std::make_pair(*res1, *res2);
    }
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

private:
  std::vector<Physical*> m_colliders;
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

    CollisionResult res = CollisionUtils::concreteCompute(target, physical);
    if (!res.has_value())
    {
      continue;
    }

    result[physical] = *res;
  }

  return result;
}
