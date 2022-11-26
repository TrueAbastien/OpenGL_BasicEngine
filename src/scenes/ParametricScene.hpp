#pragma once

#include "components/Scene.hpp"
#include "components/ParametricMesh.hpp"

class ParametricScene final : public Scene
{
public:
  ParametricScene() = default;

  const char* getName() const override
  {
    return "Parametric";
  }

  void construct(Renderer* renderer) override
  {
    Scene::construct(renderer);

    // Tube
    {
      auto tube = std::make_shared<ParametricMesh>(
        std::make_shared<ParametricTube>(5.0, 2.0));
      tube->setLocalToParent(glm::vec3(0.0, 5.0, 0.0));
      addChild(tube);
    }

    // Sphere
    {
      auto sphere = std::make_shared<ParametricMesh>(
        std::make_shared<ParametricSphere>(2.0));
      sphere->setLocalToParent(glm::vec3(0.0, -5.0, 0.0));
      addChild(sphere);
    }

    // Torus
    {
      auto torus = std::make_shared<ParametricMesh>(
        std::make_shared<ParametricTorus>(2.0, 3.0));
      torus->setLocalToParent(glm::vec3(5.0, 0.0, 0.0));
      addChild(torus);
    }
  }
};
