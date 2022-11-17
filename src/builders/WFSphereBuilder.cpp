#include <list>
#include "WFSphereBuilder.hpp"

WFSphereBuilder::WFSphereBuilder(float radius)
  : m_radius(radius)
  {
  }

  float WFSphereBuilder::getRadius() const
  {
    return m_radius;
  }

Builder::Result WFSphereBuilder::makeMeshContent(ColorationMethod coloration) const
{
  Builder::Result result;

  auto makeVertex = [coloration](const glm::vec3& pos) -> VertexType
  {
    VertexType vt = {pos, glm::normalize(pos)};
    vt.color = coloration(vt);
    return vt;
  };

  const float goldenRatio = (1.0 + std::sqrt(5.0)) / 2.0;
    
// Vertices
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-1.0, goldenRatio, 0.0)) * m_radius));   // 0
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(1.0, goldenRatio, 0.0)) * m_radius));    // 1
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-1.0, -goldenRatio, 0.0)) * m_radius));  // 2
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(1.0, -goldenRatio, 0.0)) * m_radius));   // 3
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, -1.0, goldenRatio)) * m_radius));   // 4
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, 1.0, goldenRatio)) * m_radius));    // 5
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, -1.0, -goldenRatio)) * m_radius));  // 6
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(0.0, 1.0, -goldenRatio)) * m_radius));   // 7
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(goldenRatio, 0.0, -1.0)) * m_radius));   // 8
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(goldenRatio, 0.0, 1.0)) * m_radius));    // 9
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-goldenRatio, 0.0, -1.0)) * m_radius));  // 10
  result.vertices.push_back(makeVertex(glm::normalize(glm::vec3(-goldenRatio, 0.0, 1.0)) * m_radius));   // 11


// Faces

  std::list<glm::vec3> icoFaces;
  icoFaces.push_back(glm::vec3(0, 11, 5));
  icoFaces.push_back(glm::vec3(0, 5, 1));
  icoFaces.push_back(glm::vec3(0, 1, 7));
  icoFaces.push_back(glm::vec3(0, 7, 10));
  icoFaces.push_back(glm::vec3(0, 10, 11));
  icoFaces.push_back(glm::vec3(1, 5, 9));
  icoFaces.push_back(glm::vec3(5, 11, 4));
  icoFaces.push_back(glm::vec3(11, 10, 2));
  icoFaces.push_back(glm::vec3(10, 7, 6));
  icoFaces.push_back(glm::vec3(7, 1, 8));
  icoFaces.push_back(glm::vec3(3, 9, 4));
  icoFaces.push_back(glm::vec3(3, 4, 2));
  icoFaces.push_back(glm::vec3(3, 2, 6));
  icoFaces.push_back(glm::vec3(3, 6, 8));
  icoFaces.push_back(glm::vec3(3, 8, 9));
  icoFaces.push_back(glm::vec3(4, 9, 5));
  icoFaces.push_back(glm::vec3(2, 4, 11));
  icoFaces.push_back(glm::vec3(6, 2, 10));
  icoFaces.push_back(glm::vec3(8, 6, 7));
  icoFaces.push_back(glm::vec3(9, 8, 1));

  int verticesCount = 12;

  std::list<glm::vec3> subdividedIcoFaces;
  std::list<glm::vec3> TwoTimesSubdividedIcoFaces;
  int index_0;
  int index_1;
  int index_2;
  int index_3;
  int index_4;
  int index_5;

  for (glm::vec3 face : icoFaces) {
    index_0 = face.x;
    index_1 = face.y;
    index_2 = face.z;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_0].position + result.vertices[index_1].position)) * m_radius));
    index_3 = verticesCount++;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_1].position + result.vertices[index_2].position)) * m_radius));
    index_4 = verticesCount++;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_2].position + result.vertices[index_0].position)) * m_radius));
    index_5 = verticesCount++;
    subdividedIcoFaces.push_back(glm::vec3(index_0, index_3, index_5));
    subdividedIcoFaces.push_back(glm::vec3(index_3, index_1, index_4));
    subdividedIcoFaces.push_back(glm::vec3(index_5, index_4, index_2));
    subdividedIcoFaces.push_back(glm::vec3(index_3, index_4, index_5));
  }

    for (glm::vec3 face : subdividedIcoFaces) {
    index_0 = face.x;
    index_1 = face.y;
    index_2 = face.z;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_0].position + result.vertices[index_1].position)) * m_radius));
    index_3 = verticesCount++;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_1].position + result.vertices[index_2].position)) * m_radius));
    index_4 = verticesCount++;
    result.vertices.push_back(makeVertex(glm::normalize(0.5f * (result.vertices[index_2].position + result.vertices[index_0].position)) * m_radius));
    index_5 = verticesCount++;
    TwoTimesSubdividedIcoFaces.push_back(glm::vec3(index_0, index_3, index_5));
    TwoTimesSubdividedIcoFaces.push_back(glm::vec3(index_3, index_1, index_4));
    TwoTimesSubdividedIcoFaces.push_back(glm::vec3(index_5, index_4, index_2));
    TwoTimesSubdividedIcoFaces.push_back(glm::vec3(index_3, index_4, index_5));
  }

  for (glm::vec3 face : TwoTimesSubdividedIcoFaces) {
    result.indexes.push_back(face.x); result.indexes.push_back(face.y);
    result.indexes.push_back(face.x); result.indexes.push_back(face.z);
    result.indexes.push_back(face.y); result.indexes.push_back(face.z);
  }

  return result;
}
