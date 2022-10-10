#include "Meshable.hpp"

// ------------------------------------------------------------------------------------------------
Meshable::Meshable()
  : m_vertices(0),
    m_indexes(0)
{
}

// ------------------------------------------------------------------------------------------------
void Meshable::initializeRenderable(std::vector<VertexType> vertices, std::vector<GLuint> index)
{
  m_vertices = std::move(vertices);
  m_indexes = std::move(index);

  Meshable::updateMesh();
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
void Meshable::updateMesh()
{
  if (m_vertices.empty() || m_indexes.empty())
  {
    return;
  }

  Renderable::initializeRenderable(m_vertices, m_indexes);
}
