#include "Component.hpp"
#include "Renderer.hpp"

#include "asset.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <iostream>


// ------------------------------------------------------------------------------------------------
void Component::setLocalModel(const glm::mat4& model)
{
  m_parentToLocal = model;
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalModel(const glm::vec3& trsl, const glm::vec3& rot)
{
  glm::mat4 tr = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0)) *
                 glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0)) *
                 glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
  m_parentToLocal = glm::translate(tr, trsl);
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Component::getLocalModel() const
{
  return m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
bool Component::addChild(const Pointer& child)
{
  if (containsChild(child))
  {
    return false;
  }

  m_children.push_back(child);
  return true;
}

// ------------------------------------------------------------------------------------------------
bool Component::removeChild(const Pointer& child)
{
  return std::erase(m_children, child) > 0;
}

// ------------------------------------------------------------------------------------------------
bool Component::removeChild(int index)
{
  if (index < 0 || index >= m_children.size())
  {
    return false;
  }

  m_children.erase(m_children.begin() + index);
  return true;
}

// ------------------------------------------------------------------------------------------------
Component::Pointer Component::getChild(int index) const
{
  if (index < 0 || index >= m_children.size())
  {
    return nullptr;
  }

  return m_children.at(index);
}

// ------------------------------------------------------------------------------------------------
Component::Collection Component::getChildren() const
{
  return m_children;
}

// ------------------------------------------------------------------------------------------------
bool Component::containsChild(const Pointer& child) const
{
  auto it = std::find(m_children.begin(), m_children.end(), child);

  return it != m_children.end();
}

// ------------------------------------------------------------------------------------------------
void Component::initialize(Renderer* renderer)
{
  beforeInitialize(renderer);

  for (auto& child : m_children)
  {
    child->initialize(renderer);
  }
}

// ------------------------------------------------------------------------------------------------
void Component::update(Renderer* renderer, UpdateData& data)
{
  data.worldToParent = data.worldToLocal;
  data.worldToLocal *= m_parentToLocal;

  beforeUpdate(renderer, data);

  for (auto& child : m_children)
  {
    child->update(renderer, data);
  }
}

// ------------------------------------------------------------------------------------------------
Renderable::Renderable()
  : vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER),
    shaderProgram({vertexShader, fragmentShader})
{
  glCheckError(__FILE__, __LINE__);
}

// ------------------------------------------------------------------------------------------------
void Renderable::initializeRenderable(std::vector<VertexType> vertices,
                                      std::vector<GLuint> index)
{
  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << index.size() << std::endl;

  // creation of the vertex array buffer----------------------------------------

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
               index.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // map vbo to shader attributes
  shaderProgram.setAttribute("position", 3, sizeof(VertexType),
                             offsetof(VertexType, position));
  shaderProgram.setAttribute("normal", 3, sizeof(VertexType),
                             offsetof(VertexType, normal));
  shaderProgram.setAttribute("color", 4, sizeof(VertexType),
                             offsetof(VertexType, color));

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);
}

// ------------------------------------------------------------------------------------------------
void Renderable::updateRenderable(Renderer* renderer, glm::mat4 worldToLocal, GLsizei nFaces)
{
  shaderProgram.use();

  // send uniforms
  shaderProgram.setUniform("projection", renderer->getProjection());
  shaderProgram.setUniform("view", renderer->getView() * worldToLocal);

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_TRIANGLES,     // mode
                 nFaces * 3,       // count
                 GL_UNSIGNED_INT,  // type
                 NULL              // element array buffer offset
  );

  glBindVertexArray(0);

  shaderProgram.unuse();
}

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
void Meshable::updateMesh()
{
  if (m_vertices.empty() || m_indexes.empty())
  {
    return;
  }

  Renderable::initializeRenderable(m_vertices, m_indexes);
}

// ------------------------------------------------------------------------------------------------
Box::Box(glm::vec3 scale) : Renderable(), m_scale(scale) {}

// ------------------------------------------------------------------------------------------------
glm::vec3 Box::scale() const
{
  m_scale;
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

  Renderable::initializeRenderable(vertices, index);
}

// ------------------------------------------------------------------------------------------------
void Box::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Renderable::updateRenderable(renderer, data.worldToLocal,
                               6 * // Face on Cube
                               2); // Triangle per Face
}

// ------------------------------------------------------------------------------------------------
void RigidBody::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  // Position
  m_linear_velocity += m_force * (float) (data.dt / m_mass);
  m_position += m_linear_velocity * (float) data.dt;

  // Rotation
  // TODO

  updateTransform();
  data.worldToLocal = data.worldToParent * m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::updateTransform()
{
  glm::mat4x4 mat(m_rotation);
  glm::translate(mat, m_position);

  setLocalModel(mat);
}
