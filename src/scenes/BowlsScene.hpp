#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class BowlsScene final :public Scene
{
public:
  BowlsScene() = default;

  const char* getName() const override
  {
    return "Bowls";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(40.0, 10.0, 0.2)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(-10.0, 0.0, -3.0));
      addChild(groundRB);
    }

    // Jack
    {
      auto jack = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(0.3f));
      auto jackRB = std::make_shared<RigidBody>(jack, 2.0, 0.2, false, true);
      jackRB->translateBy(glm::vec3(-22.0, 0.0, -2.0));
      addChild(jackRB);
    }

    // Bowls
    {
      auto bowl1 = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(0.6f));
      auto bowl1RB = std::make_shared<RigidBody>(bowl1, 2.0, 0.2, false, true);
      bowl1RB->translateBy(glm::vec3(-18.0, -1.0, -2.0));
      addChild(bowl1RB);
    }

    {
      auto bowl2 = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(0.6f));
      auto bowl2RB = std::make_shared<RigidBody>(bowl2, 2.0, 0.2, false, true);
      bowl2RB->translateBy(glm::vec3(0.0, 0.0, 2.0));
      bowl2RB->setInitLinearVelocity(glm::vec3(-15.0, 0.0, 2.0));
      addChild(bowl2RB);
    }
  }
};
