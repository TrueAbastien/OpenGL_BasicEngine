#pragma once

#include "Component.hpp"

class Scene final : public Component
{
public:
  Scene();

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
