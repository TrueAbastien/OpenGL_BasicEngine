#pragma once

#include "components/Meshable.hpp"

namespace algo
{
  using Simplex = std::vector<glm::vec3>;

  std::optional<Simplex> solveGJK(const Meshable* body1, const Meshable* body2);
}
