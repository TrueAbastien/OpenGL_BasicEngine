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
      auto bat = std::make_shared<BoxCollider>(
        std::make_shared<TexturedMesh>("/mesh/bat.obj", "/texture/wood.jpg",
                                       0.15,
                                       glm::vec3(-2.5, -0.5, 0.0)));
      auto batRB = std::make_shared<RigidBody>(bat, 10.0, 0.0, true, false);
      batRB->setInitAngularMomentum(glm::vec3(-2e+3f, 0.0, 0.0));
      addChild(batRB);
    }

    // Ball
    {
      auto makeBall = [&](float height)
      {
        auto ball = std::make_shared<SphereCollider>(
          std::make_shared<Sphere>(1.0f));
        auto ballRB = std::make_shared<RigidBody>(ball, 1.0, 1.0, false, true);
        ballRB->translateBy(glm::vec3(0.0, -2.0, height));
        addChild(ballRB);
      };

      makeBall(10.0);
      makeBall(20.0);
      makeBall(30.0);
    }
  }
};
