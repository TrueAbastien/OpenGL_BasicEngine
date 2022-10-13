#pragma once

#include "components/Component.hpp"

class Builder
{
public:
  struct Result
  {
    std::vector<VertexType> vertices;
    std::vector<GLuint> indexes;
  };

protected:
  virtual Result makeMeshContent() const = 0;
};