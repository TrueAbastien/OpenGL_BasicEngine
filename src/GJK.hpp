#pragma once

#include "components/Meshable.hpp"

namespace algo
{
  CollisionResult solveGJK(Meshable* body1, Meshable* body2);
}
