#pragma once

#include "Meshable.hpp"
#include "builders/SphereBuilder.hpp"

class Sphere : public Meshable, public SphereBuilder
{
public:
  Sphere(float radius);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
