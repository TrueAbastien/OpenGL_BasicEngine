#pragma once

#include "Builder.hpp"

class BoxBuilder : public Builder
{
protected:
  BoxBuilder(glm::vec3 scale);

public:
  glm::vec3 getScale() const;

protected:
  Result makeMeshContent() const override;

protected:
  glm::vec3 m_scale;
};