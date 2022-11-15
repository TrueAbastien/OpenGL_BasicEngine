#pragma once

#include "components/Physical.hpp"
#include "components/BoxCollider.hpp"
#include "components/SphereCollider.hpp"

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
      BoxCollider,
      SphereCollider
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
    auto computeA2B = [](BoxCollider* bodyA, BoxCollider* bodyB) -> std::optional<CollisionBodyData>
    {
      using Vertices = std::vector<glm::vec3>; // Set of 8 Vertices (for OBBs)
      using Interval = std::pair<float, float>; // First: Lower, Second: Upper

      // Interval of each Vertices Projection
      auto projInterval = [](const glm::vec3& axis, const Vertices& V) -> Interval
      {
        float proj = glm::dot(axis, V[0]);
        Interval result = std::make_pair(proj, proj);

        for (size_t ii = 1; ii < V.size(); ++ii)
        {
          proj = glm::dot(axis, V[ii]);
          if (proj > result.second) result.second = proj;
          else if (proj < result.first) result.first = proj;
        }

        return result;
      };

      // Is Overlapping on the specified Axis
      auto isOverlapping = [&](const glm::vec3& axis, const Vertices& A, const Vertices& B, float& s) -> bool
      {
        Interval iA = projInterval(axis, A);
        Interval iB = projInterval(axis, B);

        if (iA.first >= iB.first)
        {
          if (iA.second <= iB.second) // B Contains A
          {
            s = 0.0f;
            return true;
          }
          if (iA.first <= iB.second) // A Above B
          {
            //s = -1.0f;
            s = 1.0f;
            return true;
          }
        }
        else if (iB.first >= iA.first)
        {
          if (iB.second <= iA.second) // A Contains B
          {
            s = 0.0f;
            return true;
          }
          if (iB.first <= iA.second) // B Above A
          {
            //s = 1.0f;
            s = -1.0f;
            return true;
          }
        }

        return false;
      };

      // Access Matrix Column
      auto col = [](const glm::mat4& M, size_t i) -> glm::vec3
      {
        //return glm::vec3(M[0][i], M[1][i], M[2][i]);
        return glm::vec3(M[i]);
      };

      // Convert to Vertices
      auto getVertices = [](BoxCollider* body, const glm::mat4& localToWorld) -> Vertices
      {
        Vertices result(0);
        auto vertices = body->getVertices();

        std::transform(vertices.begin(), vertices.end(), std::back_inserter(result),
                       [&](VertexType vtx) -> glm::vec3
                       {
                         return localToWorld * glm::vec4(vtx.position, 1.0);
                       });

        return result;
      };

      // Initialize Values
      glm::mat4 A = bodyA->localToWorld();
      glm::mat4 B = bodyB->localToWorld();
      //glm::mat3 C = glm::transpose(glm::transpose(glm::mat3(A)) * glm::mat3(B));
      glm::mat3 C = glm::transpose(glm::mat3(A)) * glm::mat3(B);
      glm::vec3 D = col(B, 3) - col(A, 3);
      glm::vec3 a = bodyA->getScale() * 0.5f;
      glm::vec3 b = bodyB->getScale() * 0.5f;

      // Initialize Vertices
      Vertices A_vertices = getVertices(bodyA, A);
      Vertices B_vertices = getVertices(bodyB, B);

      std::array<float, 15> sigmas;
      float s;

      uint16_t sat_result = 1;
      size_t sat_index = 0;

      auto bindNext = [&](bool r)
      {
        sigmas[sat_index] = s;

        if (r) sat_result |= (1 << (++sat_index));
        else sat_index++;
      };

      // Face Axis
      for (size_t i = 0; i < 3; ++i)
      {
        bindNext(isOverlapping(-col(A, i), A_vertices, B_vertices, s));
      }
      for (size_t j = 0; j < 3; ++j)
      {
        bindNext(isOverlapping(col(B, j), A_vertices, B_vertices, s));
      }

      // Edge Axis
      for (size_t i = 0; i < 3; ++i)
      {
        for (size_t j = 0; j < 3; ++j)
        {
          bindNext(isOverlapping(glm::cross(col(A, i), col(B, j)), A_vertices, B_vertices, s));
        }
      }

      // Saving SAT
      uint16_t prev_sat_result = bodyA->OBBSeparatingAxis.contains(bodyB) ? bodyA->OBBSeparatingAxis[bodyB] : 0;
      bodyA->OBBSeparatingAxis[bodyB] = sat_result;

      // No Intersection
      if (sat_result != (uint16_t) ~0)
      {
        return std::nullopt;
      }

      glm::vec3 pos;
      glm::vec3 A2B_normal;

      uint16_t filter = prev_sat_result ^ ~0;

      if (filter == (uint16_t) 0)
      {
        return std::nullopt;
      }

      size_t flag;
      for (flag = 1; flag <= 15; ++flag)
      {
        if ((filter & (1 << flag)) != (uint16_t) 0)
        {
          break;
        }
      }

      // Compute Position/Normal
      s = sigmas[flag - 1];

      // - A Axis
      if (flag >= 1 && flag <= 3)
      {
        size_t i = flag - 1;

        // Position
        pos = col(B, 3);
        for (size_t j = 0; j < 3; ++j)
        {
          pos -= col(B, j) * s * glm::sign(C[i][j]) * b[j];
        }

        // Normal
        A2B_normal = -s * col(A, i);
      }

      // - B Axis
      else if (flag > 3 && flag <= 6)
      {
        size_t j = flag - 4;

        // Position
        pos = col(A, 3);
        for (size_t i = 0; i < 3; ++i)
        {
          pos += col(A, i) * s * glm::sign(C[i][j]) * a[i];
        }

        // Normal
        A2B_normal = s * col(B, j);
      }

      // - A*B Axis
      else if (flag > 6 && flag <= 15)
      {
        using Indexor = std::array<size_t, 3>;
        static constexpr std::array<Indexor, 3> indexors = {
          Indexor{ 0, 1, 2 },
          Indexor{ 1, 0, 2 },
          Indexor{ 2, 1, 0 }
        };

        size_t i = (flag - 7) / 3;
        size_t j = (flag - 7) % 3;

        Indexor ii = indexors[i];
        Indexor jj = indexors[j];

        if (C[ii[0]][jj[0]] == 0.0f)
        {
          return std::nullopt;
        }

        auto sign2 = [](size_t f, size_t s) -> float
        {
          if ((f + 1) % 3 == s) return +1.0f;
          if ((s + 1) % 3 == f) return -1.0f;
          else return 0.0f;
        };

        glm::vec3 x, y;
        {
          x[ii[1]] = s * sign2(ii[1], ii[0]) * glm::sign(C[ii[2]][jj[0]]) * a[ii[1]];
          x[ii[2]] = s * sign2(ii[2], ii[0]) * glm::sign(C[ii[1]][jj[0]]) * a[ii[2]];
          y[jj[1]] = s * sign2(jj[1], jj[0]) * glm::sign(C[ii[0]][jj[2]]) * b[jj[1]];
          y[jj[2]] = s * sign2(jj[2], jj[0]) * glm::sign(C[ii[0]][jj[1]]) * b[jj[2]];

          x[ii[0]] = (1 / (1 - glm::pow(C[ii[0]][jj[0]], 2))) *
            (glm::dot(col(A, ii[0]), D) + C[ii[0]][jj[0]] * (
              -glm::dot(col(B, jj[0]), D) +
              C[ii[1]][jj[0]] * x[ii[1]] +
              C[ii[2]][jj[0]] * x[ii[2]]) +
              C[ii[0]][jj[1]] * y[jj[1]] +
              C[ii[0]][jj[2]] * y[jj[2]]);
        }

        pos = A * glm::vec4(x, 1.0);
        A2B_normal = s * glm::cross(col(A, i), col(B, j));
      }

      // Error
      else return std::nullopt;

      std::cout << "Flag: " << flag << std::endl; //DEBUG

      // Sends out result
      return CollisionBodyData
      {
        pos, A2B_normal
      };
    };

    auto resultAB = computeA2B(body1, body2);
    if (!resultAB.has_value()) return std::nullopt;

    auto resultBA = computeA2B(body2, body1);
    if (!resultBA.has_value()) return std::nullopt;

    return std::make_pair(*resultAB, *resultBA);
  }

  // Sphere Definitions ------------------------------------------------------------------------------
  template <>
  inline int priority<SphereCollider>()
  {
    return 1;
  }

  template <>
  inline CollisionResult internalCompute<SphereCollider, SphereCollider>(SphereCollider* sphere1, SphereCollider* sphere2)
  {
    const float radiusSphere1 = sphere1->getRadius();
    const float radiusSphere2 = sphere2->getRadius();

    const glm::mat4 localToWorldSphere1 = sphere1->localToWorld();
    const glm::mat4 localToWorldSphere2 = sphere2->localToWorld();

    std::cout << "radius " << radiusSphere1 << " " << localToWorldSphere1[0][3] << localToWorldSphere1[1][3] << localToWorldSphere1[2][3] << std::endl;
    std::cout << "radius " << radiusSphere2 << " " << localToWorldSphere2[0][3] << localToWorldSphere2[1][3] << localToWorldSphere2[2][3] << std::endl;

    const glm::vec3 centerSphere1 = glm::vec3(localToWorldSphere1[0][3], localToWorldSphere1[1][3], localToWorldSphere1[2][3]);
    const glm::vec3 centerSphere2 = glm::vec3(localToWorldSphere2[0][3], localToWorldSphere2[1][3], localToWorldSphere2[2][3]);

    float distBetweenCenters = glm::length(centerSphere1 - centerSphere2);

    if (distBetweenCenters < (radiusSphere1 + radiusSphere2))
    {
      return std::nullopt;
    } 
    else 
    {
      std::cout << 1 << std::endl;

    }
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
      result[physical] = *CollisionUtils::swap(m_cachedResults[physical][target]);
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
