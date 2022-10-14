#include "BoxBuilder.hpp"

// ------------------------------------------------------------------------------------------------
BoxBuilder::BoxBuilder(glm::vec3 scale)
  : m_scale(scale)
{
}

// ------------------------------------------------------------------------------------------------
glm::vec3 BoxBuilder::getScale() const
{
  return m_scale;
}

// ------------------------------------------------------------------------------------------------
Builder::Result BoxBuilder::makeMeshContent(ColorationMethod coloration) const
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

  result.indexes.push_back(0); result.indexes.push_back(1); result.indexes.push_back(2);  // A, B, C
  result.indexes.push_back(2); result.indexes.push_back(1); result.indexes.push_back(3);  // C, B, D

  result.indexes.push_back(0); result.indexes.push_back(4); result.indexes.push_back(1);  // A, E, B
  result.indexes.push_back(1); result.indexes.push_back(4); result.indexes.push_back(5);  // B, E, F

  result.indexes.push_back(6); result.indexes.push_back(4); result.indexes.push_back(0);  // G, E, A
  result.indexes.push_back(6); result.indexes.push_back(0); result.indexes.push_back(2);  // G, A, C

  result.indexes.push_back(6); result.indexes.push_back(2); result.indexes.push_back(3);  // G, C, D
  result.indexes.push_back(6); result.indexes.push_back(3); result.indexes.push_back(7);  // G, D, H

  result.indexes.push_back(1); result.indexes.push_back(5); result.indexes.push_back(7);  // B, F, H
  result.indexes.push_back(1); result.indexes.push_back(7); result.indexes.push_back(3);  // B, H, D

  result.indexes.push_back(7); result.indexes.push_back(5); result.indexes.push_back(4);  // H, F, E
  result.indexes.push_back(7); result.indexes.push_back(4); result.indexes.push_back(6);  // H, E, G

  return result;
}
