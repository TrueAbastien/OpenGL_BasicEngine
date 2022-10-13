#pragma once

#include "Meshable.hpp"
#include "builders/BoxBuilder.hpp"

class Box : public Meshable, public BoxBuilder
{
public:
  Box(glm::vec3 scale);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
