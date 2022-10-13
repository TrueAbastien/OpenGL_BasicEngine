#include "Box.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

// ------------------------------------------------------------------------------------------------
Box::Box(glm::vec3 scale)
  : Meshable(), BoxBuilder(scale)
{
  Meshable::makeMesh(makeMeshContent());
}

// ------------------------------------------------------------------------------------------------
void Box::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

// ------------------------------------------------------------------------------------------------
void Box::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.worldToLocal,
                             6 * // Face on Cube
                             2); // Triangle per Face
}
