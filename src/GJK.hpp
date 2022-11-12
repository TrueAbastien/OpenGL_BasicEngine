#pragma once

#include "components/Meshable.hpp"

namespace algo
{
  using Simplex = std::vector<glm::vec3>;

  CollisionResult solveGJK(const Meshable* body1, const Meshable* body2);
}
