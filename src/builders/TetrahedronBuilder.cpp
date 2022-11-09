#include "TetrahedronBuilder.hpp"

#include <algorithm>
#include <numeric>

// ------------------------------------------------------------------------------------------------
TetrahedronBuilder::TetrahedronBuilder(std::vector<glm::vec3> simplex)
  : m_simplex(simplex)
{
}

// ------------------------------------------------------------------------------------------------
Builder::Result TetrahedronBuilder::makeMeshContent(ColorationMethod method) const
{
  if (m_simplex.size() != 4)
  {
    return Result();
  }

  Result res;

  glm::vec3 barycenter = 0.25f * std::accumulate(m_simplex.begin(), m_simplex.end(), glm::vec3(0.0));
  std::transform(m_simplex.begin(), m_simplex.end(), std::back_inserter(res.vertices), [&](glm::vec3 pos)
                 {
                   VertexType vtx
                   {
                     pos, // Position
                     glm::normalize(pos - barycenter) // Normal
                   };

                   vtx.color = method(vtx);
                   return vtx;
                 });

  auto modPos = [&](int index)
  {
    return m_simplex[index & 0b11];
  };
  auto modIdx = [](int index)
  {
    return (GLuint) (index & 0b11);
  };
  for (int face = 0; face < 4; ++face)
  {
    glm::vec3 normal = glm::cross(
      modPos(face + 1) - modPos(face),
      modPos(face + 2) - modPos(face));

    res.indexes.push_back(modIdx(face));
    if (glm::dot(normal, modPos(face + 3) - modPos(face)) > 0.0f)
    {
      res.indexes.push_back(modIdx(face + 2));
      res.indexes.push_back(modIdx(face + 1));
    }
    else
    {
      res.indexes.push_back(modIdx(face + 1));
      res.indexes.push_back(modIdx(face + 2));
    }
  }

  return res;
}
