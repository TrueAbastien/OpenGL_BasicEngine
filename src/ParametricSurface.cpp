#include "ParametricSurface.hpp"

#include "glm/gtc/constants.hpp"

// ------------------------------------------------------------------------------------------------
ParametricSurface::ParametricSurface(Func2D x_func, Func2D y_func, Func2D z_func)
  : m_funcs({x_func, y_func, z_func})
{
}

// ------------------------------------------------------------------------------------------------
Builder::Result ParametricSurface::makeMesh(const glm::vec2& t_min, const glm::vec2& t_max, const glm::ivec2& sampling) const
{
  Builder::Result res;

  glm::ivec2 divs = sampling - glm::ivec2(1);

  glm::vec2 step = glm::vec2(
    (t_max.x - t_min.x) / divs.x,
    (t_max.y - t_min.y) / divs.y);

  // Compute Positions
  for (int y = 0; y < sampling.y; ++y)
  {
    for (int x = 0; x < sampling.x; ++x)
    {
      glm::vec2 t = t_min + glm::vec2(step.x * x, step.y * y);
      glm::vec3 pos = computePos(t);

      res.vertices.push_back(VertexType
                             {
                               pos, glm::vec3(1.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 1.0)
                             });
    }
  }

  // Construct Wireframe Mesh
  const auto D2_D1 = [&](int x, int y) -> int
  {
    return x + y * sampling.x;
  };
  // -- Rows
  for (int y = 0; y < sampling.y; ++y)
  {
    for (int x = 0; x < sampling.x - 1; ++x)
    {
      res.indexes.push_back(D2_D1(x, y));
      res.indexes.push_back(D2_D1(x + 1, y));
    }
  }
  // -- Columns
  for (int x = 0; x < sampling.x; ++x)
  {
    for (int y = 0; y < sampling.y - 1; ++y)
    {
      res.indexes.push_back(D2_D1(x, y));
      res.indexes.push_back(D2_D1(x, y + 1));
    }
  }

  return res;
}

// ------------------------------------------------------------------------------------------------
Builder::Result ParametricSurface::makeObliviousMesh() const
{
  return Builder::Result();
}

// ------------------------------------------------------------------------------------------------
glm::vec3 ParametricSurface::computePos(const glm::vec2& uv) const
{
  return glm::vec3(
    m_funcs[0](uv),
    m_funcs[1](uv),
    m_funcs[2](uv));
}

// ------------------------------------------------------------------------------------------------
ParametricTube::ParametricTube(float height, float radius)
  : ParametricSurface(
    [=](glm::vec2 uv)
    {
      return glm::cos(glm::pi<float>() * uv.x) * radius;
    },
    [=](glm::vec2 uv)
    {
      return glm::sin(glm::pi<float>() * uv.x) * radius;
    },
    [=](glm::vec2 uv)
    {
      return uv.y * height;
    }
  )
{
}

// ------------------------------------------------------------------------------------------------
Builder::Result ParametricTube::makeObliviousMesh() const
{
  return makeMesh(glm::vec2(0.0, 0.0),
                  glm::vec2(2.0, 1.0),
                  glm::ivec2(32, 4));
}

// ------------------------------------------------------------------------------------------------
ParametricSphere::ParametricSphere(float radius)
  : ParametricSurface(
    [=](glm::vec2 uv)
    {
      return radius
        * glm::sin(glm::pi<float>() * uv.x)
        * glm::cos(glm::pi<float>() * uv.y);
    },
    [=](glm::vec2 uv)
    {
      return radius
        * glm::sin(glm::pi<float>() * uv.x)
        * glm::sin(glm::pi<float>() * uv.y);
    },
    [=](glm::vec2 uv)
    {
      return radius
        * glm::cos(glm::pi<float>() * uv.x);
    }
  )
{
}

// ------------------------------------------------------------------------------------------------
Builder::Result ParametricSphere::makeObliviousMesh() const
{
  return makeMesh(glm::vec2(0.0, 0.0),
                  glm::vec2(2.0, 1.0),
                  glm::ivec2(32, 32));
}

// ------------------------------------------------------------------------------------------------
ParametricTorus::ParametricTorus(float smallRadius, float bigRadius)
  : ParametricSurface(
    [=](glm::vec2 uv)
    {
      return glm::cos(glm::pi<float>() * uv.x)
        * (bigRadius + smallRadius
           * glm::cos(glm::pi<float>() * uv.y));
    },
    [=](glm::vec2 uv)
    {
      return smallRadius
        * glm::sin(glm::pi<float>() * uv.y);
    },
    [=](glm::vec2 uv)
    {
      return glm::sin(glm::pi<float>() * uv.x)
        * (bigRadius + smallRadius
           * glm::cos(glm::pi<float>() * uv.y));
    }
  )
{
}

// ------------------------------------------------------------------------------------------------
Builder::Result ParametricTorus::makeObliviousMesh() const
{
  return makeMesh(glm::vec2(0.0, 0.0),
                  glm::vec2(2.0, 2.0),
                  glm::ivec2(32, 32));
}
