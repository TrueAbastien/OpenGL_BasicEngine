#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class BowlingScene final : public Scene
{
public:
  BowlingScene() = default;

  const char* getName() const override
  {
    return "Bowling";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer); // IMPORTANT

    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(3.0, 20.0, 0.5)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(0.0, 0.0, -3.0));
      addChild(groundRB);
    }

    // Ball
    {
      auto ball = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(1.0));
      auto ballRB = std::make_shared<RigidBody>(ball, 10.0, 0.2, false, true);
      ballRB->translateBy(glm::vec3(0.0, -10.0, -2.0));
      ballRB->addForce(RigidBody::ExternalForce
                       {
                         glm::vec3(0.0, 0.0, 0.3), // Position
                         glm::vec3(0.0, 10.0, 0.0) // Force
                       });
      addChild(ballRB);
    }

    // Bowling Pin
    {
      auto makePin = [&](glm::vec2 pos)
      {
        auto pin = std::make_shared<BoxCollider>(
          std::make_shared<Box>(glm::vec3(0.5, 0.5, 3)));
        auto pinRB = std::make_shared<RigidBody>(pin, 5.0, 0.2, false, true);
        pinRB->translateBy(glm::vec3(pos.x, 5.0 + pos.y, 0.0));
        addChild(pinRB);
      };

      makePin(glm::vec2(0.0, 0.0));  // 001
      makePin(glm::vec2(0.5, 1.0));  // 002
      makePin(glm::vec2(-0.5, 1.0)); // 003
      makePin(glm::vec2(1.0, 2.0));  // 004
      makePin(glm::vec2(0.0, 2.0));  // 005
      makePin(glm::vec2(-1.0, 2.0)); // 006
    }
  }
};