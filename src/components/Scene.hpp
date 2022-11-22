#pragma once

#include "Component.hpp"
#include "Camera.hpp"

class Scene : public Component
{
protected:
  Scene() = default;

public:
  virtual const char* getName() const = 0;
  virtual void construct(Renderer* renderer);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
