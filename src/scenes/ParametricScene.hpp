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
        std::make_shared<ParametricTube>(3.0, 2.0));
      addChild(tube);
    }
  }
};
