#pragma once

#include "components/Physical.hpp"
#include "components/BoxCollider.hpp"

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
    //auto isColliding = [](BoxCollider* target, BoxCollider* body) -> bool
    //{
    //  glm::vec3 miScale = target->getScale() * 0.5f;
    //  glm::mat4 bodyLocal_2_TargetLocal = glm::inverse(target->localToWorld()) * body->localToWorld();

    //  const auto vertices = body->getVertices();
    //  for (const auto& vertex : vertices)
    //  {
    //    glm::vec3 pos = bodyLocal_2_TargetLocal * glm::vec4(vertex.position, 1.0);
    //    if (glm::abs(pos.x) <= miScale.x &&
    //        glm::abs(pos.y) <= miScale.y &&
    //        glm::abs(pos.z) <= miScale.z)
    //    {
    //      return true;
    //    }
    //  }

    //  return false;
    //};

    //auto closestPoint = [](BoxCollider* target, BoxCollider* body) -> CollisionBodyData
    //{
    //  glm::mat4 target_Local2World = target->localToWorld();
    //  glm::mat4 body_Local2World = body->localToWorld();
    //  glm::mat4 bodyLocal_2_TargetLocal = glm::inverse(target_Local2World) * body_Local2World;

    //  glm::vec3 targetOrigin_inBody = glm::inverse(bodyLocal_2_TargetLocal) * glm::vec4(0.0, 0.0, 0.0, 1.0);
    //  glm::vec3 miScale = body->getScale() * 0.5f;

    //  using FactorInfo = std::pair<float, glm::vec3>;
    //  FactorInfo factorInfo = std::min(
    //    {
    //      std::make_pair(miScale.x / targetOrigin_inBody.x, glm::vec3(1.0, 0.0, 0.0)),
    //      std::make_pair(miScale.y / targetOrigin_inBody.y, glm::vec3(0.0, 1.0, 0.0)),
    //      std::make_pair(miScale.z / targetOrigin_inBody.z, glm::vec3(0.0, 0.0, 1.0))
    //    },
    //    [](const FactorInfo& a, const FactorInfo& b)
    //    {
    //      return a.first < b.first;
    //    });

    //  if (factorInfo.first >= 1.0f)
    //  {
    //    return CollisionBodyData
    //    {
    //      target_Local2World * glm::vec4(0.0, 0.0, 0.0, 1.0), // Position
    //      glm::normalize((glm::vec3) (body_Local2World * glm::vec4(targetOrigin_inBody, 0.0))) // Normal
    //    };
    //  }

    //  return CollisionBodyData
    //  {
    //    body_Local2World * glm::vec4(targetOrigin_inBody * factorInfo.first, 1.0), // Position
    //    target_Local2World * glm::vec4(factorInfo.second, 0.0) // Normal
    //  };
    //};

    //if (!isColliding(body1, body2) && !isColliding(body2, body1))
    //{
    //  return std::nullopt;
    //}

    //return std::make_pair(
    //  closestPoint(body1, body2),
    //  closestPoint(body2, body1)
    //);

    struct OBBCollisionVertex
    {
      glm::vec3 localPosition;    // Vertex Position in Local Target
      glm::vec3 inBodyPosition;   // Vertex Position in Body
      bool isInBodySpace = false; // Is Vertex in Body
    };

    struct OBBCollisionEdge
    {
      OBBCollisionVertex* lowerVertex;
      OBBCollisionVertex* upperVertex;
      int nVertexInBodySpace = 0;
    };

    const auto targetInBody_collisions = [](BoxCollider* target, BoxCollider* body)
    {
      std::array<std::array<std::array<OBBCollisionVertex, 2>, 2>, 2> target_vertices; // X, Y, Z
      std::array<std::array<OBBCollisionEdge, 4>, 3> target_edges; // 3 Lower Faces (YZ, XZ, XY), 4 Normals (L -> U)

      // Vertices Initialization ------------------------------------------------------------------

      glm::vec3 target_miScale = target->getScale() * 0.5f;
      glm::vec3 body_miScale = body->getScale() * 0.5f;

      glm::mat4 targetToBody = glm::inverse(body->localToWorld()) * target->localToWorld();

      for (int x = 0; x < 2; ++x)
      {
        for (int y = 0; y < 2; ++y)
        {
          for (int z = 0; z < 2; ++z)
          {
            auto& vertex = target_vertices[x][y][z];

            glm::vec3 localPosition = target_miScale * (glm::vec3(x, y, z) * 2.0f - 1.0f);
            glm::vec3 inBodyPosition = targetToBody * glm::vec4(vertex.localPosition, 1.0);

            bool isInBodySpace =
              glm::abs(inBodyPosition.x) <= body_miScale.x &&
              glm::abs(inBodyPosition.y) <= body_miScale.y &&
              glm::abs(inBodyPosition.z) <= body_miScale.z;

            vertex = OBBCollisionVertex
            {
              localPosition,
              inBodyPosition,
              isInBodySpace
            };
          }
        }
      }

      // Target Inside Body Verification ----------------------------------------------------------

      const auto checkVerticesInBody = [&]()
      {
        for (int x = 0; x < 2; ++x)
          for (int y = 0; y < 2; ++y)
            for (int z = 0; z < 2; ++z)
              if (!target_vertices[x][y][z].isInBodySpace)
                return false;
        return true;
      };

      if (checkVerticesInBody())
      {
        // TODO: complete return
      }

      // Edges Initialization ---------------------------------------------------------------------

      const auto makeEdge = [](OBBCollisionVertex& lower, OBBCollisionVertex& upper)
      {
        return OBBCollisionEdge
        {
          &lower, &upper,
          (lower.isInBodySpace ? 1 : 0) + (upper.isInBodySpace ? 1 : 0)
        };
      };

      // Lower YZ Plane
      target_edges[0][0] = makeEdge(target_vertices[0][0][0], target_vertices[1][0][0]);
      target_edges[0][1] = makeEdge(target_vertices[0][0][1], target_vertices[1][0][1]);
      target_edges[0][2] = makeEdge(target_vertices[0][1][0], target_vertices[1][1][0]);
      target_edges[0][3] = makeEdge(target_vertices[0][1][1], target_vertices[1][1][1]);

      // Lower XZ Plane
      target_edges[1][0] = makeEdge(target_vertices[0][0][0], target_vertices[0][1][0]);
      target_edges[1][1] = makeEdge(target_vertices[0][0][1], target_vertices[0][1][1]);
      target_edges[1][2] = makeEdge(target_vertices[1][0][0], target_vertices[1][1][0]);
      target_edges[1][3] = makeEdge(target_vertices[1][0][1], target_vertices[1][1][1]);

      // Lower XZ Plane
      target_edges[2][0] = makeEdge(target_vertices[0][0][0], target_vertices[0][0][1]);
      target_edges[2][1] = makeEdge(target_vertices[0][1][0], target_vertices[0][1][1]);
      target_edges[2][2] = makeEdge(target_vertices[1][0][0], target_vertices[1][0][1]);
      target_edges[2][3] = makeEdge(target_vertices[1][1][0], target_vertices[1][1][1]);

      // TODO: edge collisions
    };
  }
}

// ------------------------------------------------------------------------------------------------
class CollisionManager final
{
public:
  using Result = std::map<Physical*, CollisionInternalResult>;
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
