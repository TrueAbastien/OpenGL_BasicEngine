#pragma once

#include "StructInfo.hpp"
#include "builders/Builder.hpp"
#include <functional>

// ------------------------------------------------------------------------------------------------
class ParametricSurface
{
public:
  using Func1D = std::function<float(float)>;
  using Func2D = std::function<float(glm::vec2)>;

protected:
  ParametricSurface(Func1D x_func,
                    Func1D y_func,
                    Func2D z_func);

public:
  virtual Builder::Result makeMesh(const glm::vec2& t_min,
                                   const glm::vec2& t_max,
                                   const glm::ivec2& sampling) const;
  virtual Builder::Result makeObliviousMesh() const;
  virtual int obliviousValueCount() const;

private:
  glm::vec3 computePos(const glm::vec2& uv) const;

private:
  Func1D m_XFunc;
  Func1D m_YFunc;
  Func2D m_ZFunc;
};

// ------------------------------------------------------------------------------------------------
class ParametricTube final : public ParametricSurface
{
public:
  ParametricTube(float height, float radius);

  Builder::Result makeObliviousMesh() const override;
  int obliviousValueCount() const override;
};
