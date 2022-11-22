#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class DominoScene final : public Scene
{
public:
  DominoScene() = default;

  const char* getName() const override
  {
    return "Domino";
  }

  // TODO
};
