#pragma once

#include "Renderable.hpp"
#include "builders/Builder.hpp"

class Meshable : public Renderable
{
protected:
  Meshable();

protected:
  void makeMesh(const Builder::Result& content);

public:
  void initializeMesh();

  std::vector<VertexType> getVertices() const;
  std::vector<GLuint> getIndexes() const;

protected:
  std::vector<VertexType> m_vertices;
  std::vector<GLuint> m_indexes;
};
