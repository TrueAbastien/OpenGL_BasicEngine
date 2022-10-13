#include "Meshable.hpp"

// ------------------------------------------------------------------------------------------------
Meshable::Meshable()
  : m_vertices(0),
  m_indexes(0)
{
}

// ------------------------------------------------------------------------------------------------
void Meshable::makeMesh(const Builder::Result& content)
{
  m_vertices = content.vertices;
  m_indexes = content.indexes;
}

// ------------------------------------------------------------------------------------------------
std::vector<VertexType> Meshable::getVertices() const
{
  return m_vertices;
}

// ------------------------------------------------------------------------------------------------
std::vector<GLuint> Meshable::getIndexes() const
{
  return m_indexes;
}

// ------------------------------------------------------------------------------------------------
void Meshable::initializeMesh()
{
  if (m_vertices.empty() || m_indexes.empty())
  {
    return;
  }

  Renderable::initializeRenderable(m_vertices, m_indexes);
}
