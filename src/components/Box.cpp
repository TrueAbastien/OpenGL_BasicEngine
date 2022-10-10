#include "Box.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

// ------------------------------------------------------------------------------------------------
Box::Box(glm::vec3 scale)
  : Physical(), m_scale(scale)
{
}

// ------------------------------------------------------------------------------------------------
glm::vec3 Box::getScale() const
{
  return m_scale;
}

// ------------------------------------------------------------------------------------------------
CurrentTargetCollisions Box::computeCollision(CollisionManager* colMan)
{
  return colMan->computeCollision(this);
}

// ------------------------------------------------------------------------------------------------
void Box::beforeInitialize(Renderer* renderer)
{
  // creation of the mesh ------------------------------------------------------
  std::vector<VertexType> vertices;
  std::vector<GLuint> index;

  //    E ----- F      Y
  //   /|      /|     /
  // A ----- B  |   O ----- X
  // |  |    |  |   |
  // |  G ---|- H   |
  // | /     | /    |
  // C ----- D      Z

  glm::vec3 v = m_scale * 0.5f;
  auto makeVertex = [](const glm::vec3& pos) -> VertexType {
    return {pos, glm::normalize(pos),
            glm::vec4(pos.x < 0 ? 1.0 : 0.5, pos.y < 0 ? 1.0 : 0.5,
                      pos.z < 0 ? 1.0 : 0.5, 1.0)};
  };
  vertices.push_back(makeVertex(glm::vec3(-v.x, -v.y, -v.z))); // A: 0
  vertices.push_back(makeVertex(glm::vec3(+v.x, -v.y, -v.z))); // B: 1
  vertices.push_back(makeVertex(glm::vec3(-v.x, -v.y, +v.z))); // C: 2
  vertices.push_back(makeVertex(glm::vec3(+v.x, -v.y, +v.z))); // D: 3
  vertices.push_back(makeVertex(glm::vec3(-v.x, +v.y, -v.z))); // E: 4
  vertices.push_back(makeVertex(glm::vec3(+v.x, +v.y, -v.z))); // F: 5
  vertices.push_back(makeVertex(glm::vec3(-v.x, +v.y, +v.z))); // G: 6
  vertices.push_back(makeVertex(glm::vec3(+v.x, +v.y, +v.z))); // H: 7

  index.push_back(0); index.push_back(1); index.push_back(2);  // A, B, C
  index.push_back(2); index.push_back(1); index.push_back(3);  // C, B, D

  index.push_back(0); index.push_back(4); index.push_back(1);  // A, E, B
  index.push_back(1); index.push_back(4); index.push_back(5);  // B, E, F

  index.push_back(6); index.push_back(4); index.push_back(0);  // G, E, A
  index.push_back(6); index.push_back(0); index.push_back(2);  // G, A, C

  index.push_back(6); index.push_back(2); index.push_back(3);  // G, C, D
  index.push_back(6); index.push_back(3); index.push_back(7);  // G, D, H

  index.push_back(1); index.push_back(5); index.push_back(7);  // B, F, H
  index.push_back(1); index.push_back(7); index.push_back(3);  // B, H, D

  index.push_back(7); index.push_back(5); index.push_back(4);  // H, F, E
  index.push_back(7); index.push_back(4); index.push_back(6);  // H, E, G

  Meshable::initializeRenderable(vertices, index);

  Physical::beforeInitialize(renderer);
}

// ------------------------------------------------------------------------------------------------
void Box::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.worldToLocal,
                             6 * // Face on Cube
                             2); // Triangle per Face
}
