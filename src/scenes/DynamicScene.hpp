#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class DynamicScene final : public Scene
{
public:
  DynamicScene() = default;

  const char* getName() const override
  {
    return "Dynamic";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Methods
    auto makeGround = [&](glm::vec3 pos, float rot)
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(2.0, 20.0, 0.5)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(pos);
      groundRB->rotateBy(glm::vec3(rot, 0.0, 0.0));
      addChild(groundRB);
    };
    auto makeSphere = [&](glm::vec3 pos, float elasticity)
    {
      auto sphere = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(1.0f));
      auto sphereRB = std::make_shared<RigidBody>(sphere, 10.0, elasticity, false, true);
      sphereRB->translateBy(pos);
      addChild(sphereRB);
    };
    auto makeBox = [&](glm::vec3 pos, float elasticity)
    {
      auto box = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0)));
      auto boxRB = std::make_shared<RigidBody>(box, 10.0, elasticity, false, true);
      boxRB->translateBy(pos);
      addChild(boxRB);
    };

    // Elasticity
    {
      makeGround(glm::vec3(0.0f), 0.0f);

      makeSphere(glm::vec3(0.0f, -8.0f, 10.0f), 1.0);
      makeSphere(glm::vec3(0.0f, -4.0f, 10.0f), 0.75);
      makeSphere(glm::vec3(0.0f, 0.0f, 10.0f), 0.5);
      makeSphere(glm::vec3(0.0f, 4.0f, 10.0f), 0.25);
      makeSphere(glm::vec3(0.0f, 8.0f, 10.0f), 0.0);
    }

    // Bounce-Off
    {
      makeGround(glm::vec3(-5.0f, 10.0f, 10.0f), 0.2);

      makeSphere(glm::vec3(-5.0f, 10.0f, 15.0f), 0.8);
      makeBox(glm::vec3(-5.0f, 10.0f, 25.0f), 0.8);
    }
  }
};
