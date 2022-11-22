#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"
#include "components/TexturedMesh.hpp"

class SpinningBatScene final : public Scene
{
public:
  SpinningBatScene() = default;

  const char* getName() const override
  {
    return "SpinningBat";
  }

  // TODO
};
