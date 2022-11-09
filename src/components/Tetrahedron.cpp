#include "Tetrahedron.hpp"

// ------------------------------------------------------------------------------------------------
Tetrahedron::Tetrahedron(std::vector<glm::vec3> simplex)
  : Meshable(), TetrahedronBuilder(simplex)
{
  int idx = -1;
  std::vector<glm::vec3> colors
  {
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0)
  };

  Meshable::makeMesh(makeMeshContent(
    [&](VertexType vt)
    {
      return glm::vec4(colors[++idx], 1.0);
    }
  ));
}

// ------------------------------------------------------------------------------------------------
void Tetrahedron::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

// ------------------------------------------------------------------------------------------------
void Tetrahedron::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             4 * // Triangles on Tetrahedron
                             3); // Values amount
}
