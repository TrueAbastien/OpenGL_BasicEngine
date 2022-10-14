#pragma once

#include "components/Component.hpp"

#include <functional>

class Builder
{
public:
  struct Result
  {
    std::vector<VertexType> vertices;
    std::vector<GLuint> indexes;
  };

  using ColorationMethod = std::function<glm::vec4(VertexType)>;

protected:
  virtual Result makeMeshContent(ColorationMethod) const = 0;
};