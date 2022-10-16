#include "Box.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

// ------------------------------------------------------------------------------------------------
Box::Box(glm::vec3 scale)
  : Meshable(), BoxBuilder(scale)
{
  Meshable::makeMesh(makeMeshContent(
    [](VertexType vt)
    {
      return glm::vec4(
        vt.position.x < 0 ? 1.0 : 0.5,
        vt.position.y < 0 ? 1.0 : 0.5,
        vt.position.z < 0 ? 1.0 : 0.5,
        1.0);
    }
  ));
}

// ------------------------------------------------------------------------------------------------
void Box::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

// ------------------------------------------------------------------------------------------------
void Box::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             6 * // Face on Cube
                             2 * // Triangle per Face
                             3); // Values amount
}
