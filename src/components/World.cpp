#include "World.hpp"

// ------------------------------------------------------------------------------------------------
World::World()
  : Meshable(), FrameBuilder()
{
  Renderable::mode = GL_LINES;

  Meshable::makeMesh(makeMeshContent(Builder::DefaultColoration));
}

// ------------------------------------------------------------------------------------------------
void World::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

// ------------------------------------------------------------------------------------------------
void World::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             3 * // Edge Amount
                             2); // Values amount
}
