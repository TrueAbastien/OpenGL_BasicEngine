#pragma once

#include "StructInfo.hpp"
#include "builders/Builder.hpp"

#include <functional>
#include <array>

// ------------------------------------------------------------------------------------------------
class ParametricSurface
{
public:
  using Func1D = std::function<float(float)>;
  using Func2D = std::function<float(glm::vec2)>;

public:
  ParametricSurface(Func2D x_func,
                    Func2D y_func,
                    Func2D z_func);

public:
  virtual Builder::Result makeMesh(const glm::vec2& t_min,
                                   const glm::vec2& t_max,
                                   const glm::ivec2& sampling) const;
  virtual Builder::Result makeObliviousMesh() const;

private:
  glm::vec3 computePos(const glm::vec2& uv) const;

private:
  std::array<Func2D, 3> m_funcs;
};

// ------------------------------------------------------------------------------------------------
class ParametricTube final : public ParametricSurface
{
public:
  ParametricTube(float height, float radius);

  Builder::Result makeObliviousMesh() const override;
};

// ------------------------------------------------------------------------------------------------
class ParametricSphere final : public ParametricSurface
{
public:
  ParametricSphere(float radius);

  Builder::Result makeObliviousMesh() const override;
};

// ------------------------------------------------------------------------------------------------
class ParametricTorus final : public ParametricSurface
{
public:
  ParametricTorus(float smallRadius, float bigRadius);

  Builder::Result makeObliviousMesh() const override;
};
