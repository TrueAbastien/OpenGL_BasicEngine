#pragma once

#include "Meshable.hpp"
#include "builders/FrameBuilder.hpp"

class World : public Meshable, public FrameBuilder
{
public:
  World();

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};
