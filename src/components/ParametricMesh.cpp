#include "ParametricMesh.hpp"

// ------------------------------------------------------------------------------------------------
ParametricMesh::ParametricMesh(const std::shared_ptr<ParametricSurface>& surface)
  : Meshable()
{
  assert(surface != nullptr);

  Renderable::mode = GL_LINES;

  m_valCount = surface->obliviousValueCount();

  Meshable::makeMesh(surface->makeObliviousMesh());
}

// ------------------------------------------------------------------------------------------------
void ParametricMesh::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

// ------------------------------------------------------------------------------------------------
void ParametricMesh::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld, (GLsizei) m_valCount);
}
