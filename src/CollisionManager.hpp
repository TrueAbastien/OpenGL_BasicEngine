#pragma once

#include "components/Physical.hpp"
#include "components/BoxCollider.hpp"

#include "GJK.hpp"

#include <algorithm>
#include <vector>
#include <numeric>
#include <optional>
#include <utility>
#include <tuple>
#include <array>

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
  inline CollisionResult physicalCastCompute(TTarget*, Physical*)
  {
    return std::nullopt;
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
      BoxCollider
      // TO COMPLETE

    >(target, body);
  }

  // Box Definitions ------------------------------------------------------------------------------
  template <>
  inline int priority<BoxCollider>()
  {
    return 0;
  }

  template <>
  inline CollisionResult internalCompute<BoxCollider, BoxCollider>(BoxCollider* body1, BoxCollider* body2)
  {
    auto simplex = algo::solveGJK(body1, body2);

    if (!simplex.has_value())
    {
      return std::nullopt;
    }

    glm::vec3 barycenter = 0.25f * std::accumulate(simplex->begin(), simplex->end(), glm::vec3(0.0));

    auto maxAxis = [](glm::vec3 v) -> glm::vec3
    {
      glm::vec3 u = glm::abs(v);

      if (u.x > u.y && u.x > u.z)
      {
        return glm::vec3(glm::sign(v.x), 0.0, 0.0);
      }
      
      if (u.y > u.x && u.y > u.z)
      {
        return glm::vec3(0.0, glm::sign(v.y), 0.0);
      }

      return glm::vec3(0.0, 0.0, glm::sign(v.z));
    };

    auto normalProj = [&](BoxCollider* target) -> glm::vec3
    {
      glm::mat4 localToWorld = target->localToWorld();

      glm::vec3 barycenterProj = glm::inverse(localToWorld) * glm::vec4(barycenter, 1.0);
      glm::vec3 miScale = 0.5f * target->getScale();
      glm::vec3 scaledProj = barycenterProj / miScale;

      return localToWorld * glm::vec4(maxAxis(scaledProj), 0.0);
    };

    auto contact = [&](BoxCollider* target) -> CollisionBodyData
    {
      glm::vec3 normal = normalProj(target);

      auto compare = [&](glm::vec3 a, glm::vec3 b)
      {
        return glm::dot(normal, a - barycenter) > glm::dot(normal, b - barycenter);
      };

      glm::vec3 position = *std::max_element(simplex->begin(), simplex->end(), compare);

      return CollisionBodyData
      {
        position,
        normal
      };
    };

    return std::make_pair(contact(body2), contact(body1));
  }
}

// ------------------------------------------------------------------------------------------------
class CollisionManager final
{
public:
  using Result = std::map<Physical*, CollisionManifold>;
  using ResultMap = std::map<Physical*, Result>;

public:
  CollisionManager();

  bool addPhysical(Physical* physical);
  bool removePhysical(Physical* physical);

  template <CollisionUtils::PhysicalDerived T>
  Result computeTargetCollisions(T* target);

  ResultMap computeAllCollisions();

private:
  std::vector<Physical*> m_colliders;
  ResultMap m_cachedResults;
};

// ------------------------------------------------------------------------------------------------
template<CollisionUtils::PhysicalDerived T>
inline CollisionManager::Result CollisionManager::computeTargetCollisions(T* target)
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
