#include "WFBoxBuilder.hpp"

// ------------------------------------------------------------------------------------------------
WFBoxBuilder::WFBoxBuilder(glm::vec3 scale)
  : m_scale(scale)
{
}

// ------------------------------------------------------------------------------------------------
glm::vec3 WFBoxBuilder::getScale() const
{
  return m_scale;
}

// ------------------------------------------------------------------------------------------------
Builder::Result WFBoxBuilder::makeMeshContent(ColorationMethod coloration) const
{
  Builder::Result result;

  //    E ----- F      Y
  //   /|      /|     /
  // A ----- B  |   O ----- X
  // |  |    |  |   |
  // |  G ---|- H   |
  // | /     | /    |
  // C ----- D      Z

  glm::vec3 v = m_scale * 0.5f;
  auto makeVertex = [coloration](const glm::vec3& pos) -> VertexType
  {
    VertexType vt = {pos, glm::normalize(pos)};
    vt.color = coloration(vt);
    return vt;
  };
  
  result.vertices.push_back(makeVertex(glm::vec3(-v.x, -v.y, -v.z))); // A: 0
  result.vertices.push_back(makeVertex(glm::vec3(+v.x, -v.y, -v.z))); // B: 1
  result.vertices.push_back(makeVertex(glm::vec3(-v.x, -v.y, +v.z))); // C: 2
  result.vertices.push_back(makeVertex(glm::vec3(+v.x, -v.y, +v.z))); // D: 3
  result.vertices.push_back(makeVertex(glm::vec3(-v.x, +v.y, -v.z))); // E: 4
  result.vertices.push_back(makeVertex(glm::vec3(+v.x, +v.y, -v.z))); // F: 5
  result.vertices.push_back(makeVertex(glm::vec3(-v.x, +v.y, +v.z))); // G: 6
  result.vertices.push_back(makeVertex(glm::vec3(+v.x, +v.y, +v.z))); // H: 7

  result.indexes.push_back(0); result.indexes.push_back(1);  // A, B
  result.indexes.push_back(1); result.indexes.push_back(3);  // B, D
  result.indexes.push_back(3); result.indexes.push_back(2);  // D, C
  result.indexes.push_back(2); result.indexes.push_back(0);  // C, A

  result.indexes.push_back(4); result.indexes.push_back(5);  // E, F
  result.indexes.push_back(5); result.indexes.push_back(7);  // F, H
  result.indexes.push_back(7); result.indexes.push_back(6);  // H, G
  result.indexes.push_back(6); result.indexes.push_back(4);  // G, E

  result.indexes.push_back(0); result.indexes.push_back(4);  // A, E
  result.indexes.push_back(1); result.indexes.push_back(5);  // B, F
  result.indexes.push_back(2); result.indexes.push_back(6);  // C, G
  result.indexes.push_back(3); result.indexes.push_back(7);  // D, H

  return result;
}
