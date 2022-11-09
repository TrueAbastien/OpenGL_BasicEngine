#pragma once

#include "Meshable.hpp"
#include "builders/TetrahedronBuilder.hpp"

class Tetrahedron : public Meshable, public TetrahedronBuilder
{
public:
  Tetrahedron(std::vector<glm::vec3> simplex);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};