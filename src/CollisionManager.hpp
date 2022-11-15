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
    //return algo::solveGJK(body1, body2);

    using Vertices = std::vector<glm::vec3>; // Set of 8 Vertices (for OBBs)
    using Interval = std::pair<float, float>; // First: Lower, Second: Upper
    using Coefficients = std::array<std::optional<float>, 3>;

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
    auto isOverlapping = [&](const glm::vec3& axis, const Vertices& A, const Vertices& B, float& overlap) -> bool
    {
      Interval iA = projInterval(axis, A);
      Interval iB = projInterval(axis, B);

      if (iA.first > iB.first)
      {
        if (iA.second < iB.second) // B Contains A
        {
          overlap = iA.second - iA.first;
          return true;
        }
        if (iA.first < iB.second) // B Intersects A
        {
          overlap = iB.second - iA.first;
          return true;
        }
      }
      else if (iB.first > iA.first)
      {
        if (iB.second < iA.second) // A Contains B
        {
          overlap = iB.second - iB.first;
          return true;
        }
        if (iB.first < iA.second) // A Intersects B
        {
          overlap = iA.second - iB.first;
          return true;
        }
      }

      return false;
    };

    // Access Matrix Column
    auto col = [](const glm::mat4& M, size_t i) -> glm::vec3
    {
      return glm::vec3(M[0][i], M[1][i], M[2][i]);
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
    glm::mat4 A = body1->localToWorld();
    glm::mat4 B = body2->localToWorld();
    glm::mat3 C = A * glm::inverse(B);
    glm::vec3 D = col(B, 3) - col(A, 3);
    glm::vec3 a = body1->getScale() * 0.5f;
    glm::vec3 b = body2->getScale() * 0.5f;
    
    // Initialize Vertices
    Vertices A_vertices = getVertices(body1, A);
    Vertices B_vertices = getVertices(body2, B);

    glm::vec3 pos;
    glm::vec3 A2B_normal;
    float maxOverlap = 0.0f, overlap;

    // Face Axis
    for (size_t i = 0; i < 3; ++i)
    {
      if (isOverlapping(col(A, i), A_vertices, B_vertices, overlap))
      {
        if (overlap > maxOverlap)
        {
          maxOverlap = overlap;
          // TODO: normal, position
        }
      }
      else return std::nullopt;
    }
    for (size_t j = 0; j < 3; ++j)
    {
      if (isOverlapping(col(B, j), A_vertices, B_vertices, overlap))
      {
        if (overlap > maxOverlap)
        {
          maxOverlap = overlap;
          // TODO: normal, position
        }
      }
      else return std::nullopt;
    }

    // Edge Axis
    //auto computeEdgeInfo = [&](size_t aI, size_t bI)
    //{
    //  glm::vec3 Ai = col(A, aI);
    //  glm::vec3 Bi = col(B, bI);

    //  glm::vec3 x, y;
    //  float sign;

    //  // x
    //  sign = ((aI % 2) == 1) ? -1.0f : 1.0f;
    //  for (size_t i = 0; i < 3; ++i)
    //  {
    //    if (i == aI) continue;

    //    float v = sign * glm::sign(C[(3 - i) % 3][bI]);
    //    x[i] = v * a[i];

    //    sign *= -1.0f;
    //  }
    //  
    //  // y
    //  sign = ((bI % 2) == 1) ? -1.0f : 1.0f;
    //  for (size_t j = 0; j < 3; ++j)
    //  {
    //    if (j == bI) continue;

    //    float v = sign * glm::sign(C[aI][(3 - j) % 3]);
    //    y[j] = v * b[j];

    //    sign *= -1.0f;
    //  }

    //  // x
    //  x[aI] = (1 / (1 ))
    //};

    // A0 x B0
    if (isOverlapping(glm::cross(col(A, 0), col(B, 0)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x00, y00;
      {
        x00[1] = -glm::sign(C[2][0]) * a[1];
        x00[2] = +glm::sign(C[1][0]) * a[2];
        y00[1] = -glm::sign(C[0][2]) * b[1];
        y00[2] = +glm::sign(C[0][1]) * b[2];
        x00[0] = (1 / (1 + glm::pow(C[0][0], 2)))
          * (glm::dot(col(A, 0), D) + C[0][0]
             * (glm::dot(-col(B, 0), D)
                + C[1][0] * x00[1]
                + C[2][0] * x00[2])
             + C[0][1] * y00[1]
             + C[0][2] * y00[2]);
      }
    }

    // A0 x B1
    if (isOverlapping(glm::cross(col(A, 0), col(B, 1)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x01, y01;
      {
        x01[1] = -glm::sign(C[2][1]) * a[1];
        x01[2] = +glm::sign(C[1][1]) * a[2];
        y01[0] = +glm::sign(C[0][2]) * b[0];
        y01[2] = -glm::sign(C[0][0]) * b[2];
        x01[0] = (1 / (1 + glm::pow(C[0][1], 2)))
          * (glm::dot(col(A, 0), D) + C[0][1]
             * (glm::dot(-col(B, 1), D)
                + C[1][1] * x01[1]
                + C[2][1] * x01[2])
             + C[0][0] * y01[0]
             + C[0][2] * y01[2]);
      }
    }

    // A0 x B2
    if (isOverlapping(glm::cross(col(A, 0), col(B, 2)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x02, y02;
      {
        x02[1] = -glm::sign(C[2][2]) * a[1];
        x02[2] = +glm::sign(C[1][2]) * a[2];
        y02[0] = -glm::sign(C[0][2]) * b[0];
        y02[1] = +glm::sign(C[0][0]) * b[1];
        x02[0] = (1 / (1 + glm::pow(C[0][2], 2)))
          * (glm::dot(col(A, 0), D) + C[0][2]
             * (glm::dot(-col(B, 2), D)
                + C[1][2] * x02[1]
                + C[2][2] * x02[2])
             + C[0][0] * y02[0]
             + C[0][1] * y02[1]);
      }
    }

    // A1 x B0
    if (isOverlapping(glm::cross(col(A, 1), col(B, 0)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x10, y10;
      {
        x10[0] = +glm::sign(C[2][0]) * a[0];
        x10[2] = -glm::sign(C[0][0]) * a[2];
        y10[1] = -glm::sign(C[1][2]) * b[1];
        y10[2] = +glm::sign(C[1][1]) * b[2];
        x10[1] = (1 / (1 + glm::pow(C[1][0], 2)))
          * (glm::dot(col(A, 1), D) + C[1][0]
             * (glm::dot(-col(B, 0), D)
                + C[0][0] * x10[0]
                + C[2][0] * x10[2])
             + C[1][1] * y10[1]
             + C[1][2] * y10[2]);
      }
    }

    // A1 x B1
    if (isOverlapping(glm::cross(col(A, 1), col(B, 1)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x11, y11;
      {
        x11[0] = +glm::sign(C[2][1]) * a[0];
        x11[2] = -glm::sign(C[0][1]) * a[2];
        y11[0] = +glm::sign(C[1][2]) * b[0];
        y11[2] = -glm::sign(C[1][0]) * b[2];
        x11[1] = (1 / (1 + glm::pow(C[1][1], 2)))
          * (glm::dot(col(A, 1), D) + C[1][1]
             * (glm::dot(-col(B, 1), D)
                + C[0][1] * x11[0]
                + C[2][1] * x11[2])
             + C[1][0] * y11[0]
             + C[1][2] * y11[2]);
      }
    }

    // A1 x B2
    if (isOverlapping(glm::cross(col(A, 1), col(B, 2)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x12, y12;
      {
        x12[0] = +glm::sign(C[2][2]) * a[0];
        x12[2] = -glm::sign(C[0][2]) * a[2];
        y12[0] = -glm::sign(C[1][1]) * b[0];
        y12[1] = +glm::sign(C[1][0]) * b[1];
        x12[1] = (1 / (1 + glm::pow(C[1][2], 2)))
          * (glm::dot(col(A, 1), D) + C[1][2]
             * (glm::dot(-col(B, 2), D)
                + C[0][2] * x12[0]
                + C[2][2] * x12[2])
             + C[1][0] * y12[0]
             + C[1][1] * y12[1]);
      }
    }

    // A2 x B0
    if (isOverlapping(glm::cross(col(A, 2), col(B, 0)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x20, y20;
      {
        x20[0] = -glm::sign(C[1][0]) * a[0];
        x20[1] = +glm::sign(C[0][0]) * a[2];
        y20[1] = -glm::sign(C[2][2]) * b[1];
        y20[2] = +glm::sign(C[2][1]) * b[2];
        x20[2] = (1 / (1 + glm::pow(C[2][0], 2)))
          * (glm::dot(col(A, 2), D) + C[2][0]
             * (glm::dot(-col(B, 0), D)
                + C[0][0] * x20[0]
                + C[1][0] * x20[1])
             + C[2][1] * y20[1]
             + C[2][2] * y20[2]);
      }
    }

    // A2 x B1
    if (isOverlapping(glm::cross(col(A, 2), col(B, 1)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x21, y21;
      {
        x21[0] = -glm::sign(C[1][1]) * a[0];
        x21[1] = +glm::sign(C[0][1]) * a[2];
        y21[0] = +glm::sign(C[2][2]) * b[0];
        y21[2] = -glm::sign(C[2][0]) * b[2];
        x21[2] = (1 / (1 + glm::pow(C[2][1], 2)))
          * (glm::dot(col(A, 2), D) + C[2][1]
             * (glm::dot(-col(B, 1), D)
                + C[0][1] * x21[0]
                + C[1][1] * x21[1])
             + C[2][0] * y21[0]
             + C[2][2] * y21[2]);
      }
    }

    // A2 x B2
    if (isOverlapping(glm::cross(col(A, 2), col(B, 2)), A_vertices, B_vertices, overlap))
    {
      glm::vec3 x22, y22;
      {
        x22[0] = -glm::sign(C[1][2]) * a[0];
        x22[1] = +glm::sign(C[0][2]) * a[2];
        y22[0] = -glm::sign(C[2][1]) * b[0];
        y22[1] = +glm::sign(C[2][0]) * b[1];
        x22[2] = (1 / (1 + glm::pow(C[2][2], 2)))
          * (glm::dot(col(A, 2), D) + C[2][2]
             * (glm::dot(-col(B, 2), D)
                + C[0][2] * x22[0]
                + C[1][2] * x22[1])
             + C[2][0] * y22[0]
             + C[2][1] * y22[1]);
      }
    }
    // TODO

    // Sends out result
    return std::make_pair(
      CollisionBodyData
      {
        pos, A2B_normal
      },
      CollisionBodyData
      {
        pos, -A2B_normal
      });
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
