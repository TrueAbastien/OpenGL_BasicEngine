#pragma once

#include "components/Meshable.hpp"

namespace algo
{
  CollisionResult solveGJK(const Meshable* body1, const Meshable* body2);
}
