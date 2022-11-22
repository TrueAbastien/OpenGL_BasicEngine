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

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Bat
    {
      auto bat = std::make_shared<TexturedMesh>("/mesh/bat.obj", "/texture/wood.jpg", 0.1);
      addChild(bat);
    }
  }
};
