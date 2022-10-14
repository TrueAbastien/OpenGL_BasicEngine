#pragma once

#include "Builder.hpp"

class WFBoxBuilder : public Builder
{
protected:
  WFBoxBuilder(glm::vec3 scale);

public:
  glm::vec3 getScale() const;

protected:
  Result makeMeshContent(ColorationMethod coloration) const override;

protected:
  glm::vec3 m_scale;
};