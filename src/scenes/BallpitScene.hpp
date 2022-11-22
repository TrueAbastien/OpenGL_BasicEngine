#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class BallpitScene final : public Scene
{
public:
  BallpitScene() = default;

  const char* getName() const override
  {
    return "Ballpit";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(10.0, 10.0, 0.5)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(0.0, 0.0, -5.0));
      addChild(groundRB);
    }

    // Wall1
    {
      auto wall = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(0.5, 10.0, 10.0)));
      auto wallRB = std::make_shared<RigidBody>(wall, 1e+6, 0.0, true, false);
      wallRB->translateBy(glm::vec3(-5.0, 0.0, 0.0));
      addChild(wallRB);
    }

    // Wall2
    {
      auto wall = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(0.5, 10.0, 10.0)));
      auto wallRB = std::make_shared<RigidBody>(wall, 1e+6, 0.0, true, false);
      wallRB->translateBy(glm::vec3(5.0, 0.0, 0.0));
      addChild(wallRB);
    }

    // Wall3
    {
      auto wall = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(10.0, 0.5, 10.0)));
      auto wallRB = std::make_shared<RigidBody>(wall, 1e+6, 0.0, true, false);
      wallRB->translateBy(glm::vec3(0.0, -5.0, 0.0));
      addChild(wallRB);
    }

    // Wall4
    {
      auto wall = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(10.0, 0.5, 10.0)));
      auto wallRB = std::make_shared<RigidBody>(wall, 1e+6, 0.0, true, false);
      wallRB->translateBy(glm::vec3(0.0, 5.0, 0.0));
      addChild(wallRB);
    }

    // Balls
    {
      for (int ii = 0; ii < 20; ++ii)
      {
        auto ball = std::make_shared<SphereCollider>(
          std::make_shared<Sphere>(1.0f));
        auto ballRB = std::make_shared<RigidBody>(ball, 10.0, 0.8, false, true);
        ballRB->translateBy(glm::vec3(
          (((ii + 0) * 3) % 10) / 2.0 - 2.5,
          (((ii + 1) * 3) % 10) / 2.0 - 2.5,
          ii + 3
        ));
        addChild(ballRB);
      }
    }
  }
};
