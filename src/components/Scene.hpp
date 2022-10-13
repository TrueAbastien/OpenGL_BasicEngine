#pragma once

#include "Component.hpp"
#include "Camera.hpp"

class Scene final : public Component
{
public:
  Scene(Renderer* renderer);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
