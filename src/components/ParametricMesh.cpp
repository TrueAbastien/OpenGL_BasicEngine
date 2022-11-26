#include "ParametricMesh.hpp"

// ------------------------------------------------------------------------------------------------
ParametricMesh::ParametricMesh(const std::shared_ptr<ParametricSurface>& surface)
  : Meshable()
{
  assert(surface != nullptr);

  Renderable::mode = GL_LINES;

  Builder::Result mesh = surface->makeObliviousMesh();

  m_valCount = mesh.indexes.size();
  Meshable::makeMesh(mesh);
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
