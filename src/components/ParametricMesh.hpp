#pragma once

#include "Meshable.hpp"
#include "ParametricSurface.hpp"

class ParametricMesh : public Meshable
{
public:
  ParametricMesh(const std::shared_ptr<ParametricSurface>& surface);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  int m_valCount;
};
