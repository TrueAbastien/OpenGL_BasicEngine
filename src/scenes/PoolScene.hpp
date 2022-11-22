#pragma once

#include "components/Scene.hpp"
#include "components/RigidBody.hpp"
#include "components/BoxCollider.hpp"
#include "components/Box.hpp"
#include "components/SphereCollider.hpp"
#include "components/Sphere.hpp"

class PoolScene final :public Scene
{
public:
  PoolScene() = default;

  const char* getName() const override
  {
    return "Pool";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(20.0, 10.0, 0.2)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(0.0, 0.0, -3.0));
      addChild(groundRB);
    }

    // Walls
    {
      auto wall1 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0, 7.0, 1.0)));
      auto wall1RB = std::make_shared<RigidBody>(wall1, 1e+6, 0.0, true, false);
      wall1RB->translateBy(glm::vec3(-10.0, 0.0, -2.5));
      addChild(wall1RB);
    }

   {
      auto wall2 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0, 7.0, 1.0)));
      auto wall2RB = std::make_shared<RigidBody>(wall2, 1e+6, 0.0, true, false);
      wall2RB->translateBy(glm::vec3(10.0, 0.0, -2.5));
      addChild(wall2RB);
    }

    {
      auto wall3 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(16.5, 1.0, 1.0)));
      auto wall3RB = std::make_shared<RigidBody>(wall3, 1e+6, 0.0, true, false);
      wall3RB->translateBy(glm::vec3(0.0, -5.0, -2.5));
      addChild(wall3RB);
    }

    {
      auto wall4 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(16.5, 1.0, 1.0)));
      auto wall4RB = std::make_shared<RigidBody>(wall4, 1e+6, 0.0, true, false);
      wall4RB->translateBy(glm::vec3(0.0, 5.0, -2.5));
      addChild(wall4RB);
    }

    // White Ball
    {
      auto whiteBall = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(0.5f));
      auto whiteBallRB = std::make_shared<RigidBody>(whiteBall, 5.0, 0.2, false, true);
      whiteBallRB->translateBy(glm::vec3(7.0, 0.0, -2.5));
      whiteBallRB->setInitLinearVelocity(glm::vec3(-18.0, 36.0, 0.0));
      addChild(whiteBallRB);
    }

    // Balls
    {
      auto makeBin = [&](glm::vec2 pos)
      {
        auto ball = std::make_shared<SphereCollider>(
          std::make_shared<Sphere>(0.5));
        auto ballRB = std::make_shared<RigidBody>(ball, 5.0, 0.2, false, true);
        ballRB->translateBy(glm::vec3(-3.0 + pos.x, pos.y, -2.5));
        addChild(ballRB);
      };

      makeBin(glm::vec2(0.0, 0.0));   // 001
      makeBin(glm::vec2(-1.0, 0.5));  // 002
      makeBin(glm::vec2(-1.0, -0.5)); // 003
      makeBin(glm::vec2(-2.0, 1.0));  // 004
      makeBin(glm::vec2(-2.0, 0.0));  // 005
      makeBin(glm::vec2(-2.0, -1.0)); // 006
    }
  }
};
