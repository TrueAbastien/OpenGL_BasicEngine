#pragma once

#include "Renderable.hpp"

class Meshable : public Renderable
{
protected:
  Meshable();

protected:
  void initializeRenderable(std::vector<VertexType> vertices, std::vector<GLuint> index) override;

public:
  void updateMesh();

  std::vector<VertexType> getVertices() const;
  std::vector<GLuint> getIndexes() const;

protected:
  std::vector<VertexType> m_vertices;
  std::vector<GLuint> m_indexes;
};
