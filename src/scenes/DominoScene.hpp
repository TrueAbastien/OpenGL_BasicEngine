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

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(2.0, 20.0, 0.5)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(0.0, 0.0, -2.0));
      addChild(groundRB);
    }

    // Dominos
    {
      auto makeDomino = [&](float offset)
      {
        auto box = std::make_shared<BoxCollider>(
          std::make_shared<Box>(glm::vec3(1.2, 0.3, 3.0)));
        auto boxRB = std::make_shared<RigidBody>(box, 1.0, 0.0, false, true);
        boxRB->translateBy(glm::vec3(0.0, offset, 0.0));
        addChild(boxRB);

        return boxRB;
      };

      makeDomino(-8.0f)->addForce(RigidBody::ExternalForce
                                  {
                                    glm::vec3(0.0, 0.0, 1.2),
                                    glm::vec3(0.0, 1.0, 0.0)
                                  });

      for (int ii = 1; ii < 9; ++ii)
      {
        makeDomino(ii * 2.0f - 8.0f);
      }
    }
  }
};
