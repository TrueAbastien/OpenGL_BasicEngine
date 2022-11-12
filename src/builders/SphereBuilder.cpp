#include "SphereBuilder.hpp"

SphereBuilder::SphereBuilder(float radius)
  : m_radius(radius)
  {
  }

  float SphereBuilder::getRadius() const
  {
    return m_radius;
  }

Builder::Result SphereBuilder::makeMeshContent(ColorationMethod coloration) const
{
  Builder::Result result;

  auto makeVertex = [coloration](const glm::vec3& pos) -> VertexType
  {
    VertexType vt = {pos, glm::normalize(pos)};
    vt.color = coloration(vt);
    return vt;
  };

  const double t = (1.0 + std::sqrt(5.0)) / 2.0;
  
// Vertices
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-1.0, t, 0.0))));   // 0
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(1.0, t, 0.0))));    // 1
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-1.0, -t, 0.0))));  // 2
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(1.0, -t, 0.0))));   // 3
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, -1.0, t))));   // 4
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, 1.0, t))));    // 5
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, -1.0, -t))));  // 6
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, 1.0, -t))));   // 7
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(t, 0.0, -1.0))));   // 8
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(t, 0.0, 1.0))));    // 9
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-t, 0.0, -1.0))));  // 10
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-t, 0.0, 1.0))));   // 11

// Faces
  result.indexes.push_back(0); result.indexes.push_back(11); result.indexes.push_back(5);
  result.indexes.push_back(0); result.indexes.push_back(5); result.indexes.push_back(1);
  result.indexes.push_back(0); result.indexes.push_back(1); result.indexes.push_back(7);
  result.indexes.push_back(0); result.indexes.push_back(7); result.indexes.push_back(10);
  result.indexes.push_back(0); result.indexes.push_back(10); result.indexes.push_back(11);
  result.indexes.push_back(1); result.indexes.push_back(5); result.indexes.push_back(9);
  result.indexes.push_back(5); result.indexes.push_back(11); result.indexes.push_back(4);
  result.indexes.push_back(11); result.indexes.push_back(10); result.indexes.push_back(2);
  result.indexes.push_back(010); result.indexes.push_back(7); result.indexes.push_back(6);
  result.indexes.push_back(7); result.indexes.push_back(1); result.indexes.push_back(8);
  result.indexes.push_back(3); result.indexes.push_back(9); result.indexes.push_back(4);
  result.indexes.push_back(3); result.indexes.push_back(4); result.indexes.push_back(2);
  result.indexes.push_back(3); result.indexes.push_back(2); result.indexes.push_back(6);
  result.indexes.push_back(3); result.indexes.push_back(6); result.indexes.push_back(8);
  result.indexes.push_back(3); result.indexes.push_back(8); result.indexes.push_back(9);
  result.indexes.push_back(4); result.indexes.push_back(9); result.indexes.push_back(5);
  result.indexes.push_back(2); result.indexes.push_back(4); result.indexes.push_back(11);
  result.indexes.push_back(6); result.indexes.push_back(2); result.indexes.push_back(10);
  result.indexes.push_back(8); result.indexes.push_back(6); result.indexes.push_back(7);
  result.indexes.push_back(9); result.indexes.push_back(8); result.indexes.push_back(1);

  return result;
}
