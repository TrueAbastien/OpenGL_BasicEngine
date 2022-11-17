#include "FrameBuilder.hpp"

// ------------------------------------------------------------------------------------------------
FrameBuilder::FrameBuilder()
{
}

// ------------------------------------------------------------------------------------------------
FrameBuilder::Result FrameBuilder::makeMeshContent(ColorationMethod) const
{
  FrameBuilder::Result result;

  auto makeVertex = [](const glm::vec3& pos, const glm::vec3& color) -> VertexType
  {
    return VertexType{pos, pos, glm::vec4(color, 1.0)};
  };

  result.vertices.push_back(makeVertex(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
  result.vertices.push_back(makeVertex(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
  result.vertices.push_back(makeVertex(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
  result.vertices.push_back(makeVertex(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
  result.vertices.push_back(makeVertex(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
  result.vertices.push_back(makeVertex(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));

  for (int ii = 0; ii < 6; ++ii)
  {
    result.indexes.push_back(ii);
  }

  return result;
}
