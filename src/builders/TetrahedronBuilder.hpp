#pragma once

#include "Builder.hpp"

class TetrahedronBuilder : public Builder
{
public:
  TetrahedronBuilder(std::vector<glm::vec3> simplex);

protected:
  Result makeMeshContent(ColorationMethod) const override;

protected:
  std::vector<glm::vec3> m_simplex;
};