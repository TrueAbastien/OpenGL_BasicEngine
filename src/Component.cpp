#include "Component.hpp"

#include "Renderer.hpp"
#include "CollisionManager.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "asset.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

static Camera* mainCamera = nullptr;

namespace CameraDefinitions
{
  const glm::vec2 linearVelocity = glm::vec2(1.0, 1.0);
  const glm::vec2 angularVelocity = glm::vec2(glm::pi<float>() / 10'000.0, glm::pi<float>() / 10'000.0);
  const float scrollVelocity = 0.5f;
}

// ------------------------------------------------------------------------------------------------
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (mainCamera) mainCamera->mouseCallback(xpos, ypos);
}

// ------------------------------------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (mainCamera) mainCamera->scrollBack((float) yoffset);
}

// ------------------------------------------------------------------------------------------------
void inputCallback(GLFWwindow* window, double dt)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);


  if (mainCamera) mainCamera->processInput(
    ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) ? (1 << 1) : 0) |
    ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? (1 << 2) : 0) |
    ((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) ? (1 << 3) : 0) |
    ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ? (1 << 4) : 0),
    dt);
}

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
Scene::Scene()
{
  //addChild(std::make_shared<Camera>());
}

// ------------------------------------------------------------------------------------------------
void Scene::beforeInitialize(Renderer* renderer)
{
  if (mainCamera != nullptr)
  {
    GLFWwindow* window = renderer->getWindow();

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
  }
}

// ------------------------------------------------------------------------------------------------
Camera::Camera() :
  m_position(glm::vec3(0.0)),
  m_polar   (glm::vec3(0.0)),
  m_distance(1.0f),

  m_up     (glm::vec3(0.0, 0.0, 1.0)),
  m_forward(glm::vec3(1.0, 0.0, 0.0)),
  m_right  (glm::vec3(0.0, 1.0, 0.0)),

  m_resetMouse(true),
  m_lastX     (0.0),
  m_lastY     (0.0)
{
  mainCamera = this;
}

// ------------------------------------------------------------------------------------------------
void Camera::processInput(unsigned char press, float dt)
{
  using namespace CameraDefinitions;

  if ((press & (1 << 1)) != 0)
    m_position += m_up * linearVelocity.y * dt;
  if ((press & (1 << 2)) != 0)
    m_position -= m_up * linearVelocity.y * dt;
  if ((press & (1 << 3)) != 0)
    m_position -= m_right * linearVelocity.x * dt;
  if ((press & (1 << 4)) != 0)
    m_position += m_right * linearVelocity.x * dt;
}

// ------------------------------------------------------------------------------------------------
void Camera::mouseCallback(double xpos, double ypos)
{
  using namespace CameraDefinitions;

  if (m_resetMouse)
  {
    m_lastX = xpos;
    m_lastY = ypos;
    m_resetMouse = false;
  }

  m_polar += glm::vec2(
    angularVelocity.x * (float)(xpos - m_lastX),
    angularVelocity.y * (float)(m_lastY - ypos));

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  float maxInclinaison = glm::pi<float>() / 2.0f;
  if (m_polar.x > maxInclinaison)
    m_polar.x = maxInclinaison;
  else if (m_polar.x < -maxInclinaison)
    m_polar.x = -maxInclinaison;
}

// ------------------------------------------------------------------------------------------------
void Camera::scrollBack(float yoffset)
{
  m_distance -= yoffset;

  if (m_distance < 0.1f)
    m_distance = 0.1f;
  else if (m_distance > 100.0f)
    m_distance = 100.0f;
}

// ------------------------------------------------------------------------------------------------
void Camera::beforeInitialize(Renderer* renderer)
{
  // TODO: compute distance ?
}

// ------------------------------------------------------------------------------------------------
void Camera::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  // Process Inputs
  inputCallback(renderer->getWindow(), data.dt);

  // Update Transforms
  glm::mat4 mat = computeView();
  setLocalModel(glm::inverse(mat));
  renderer->setView(mat);
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Camera::computeView()
{
  m_forward = glm::vec3(
    glm::sin(m_polar.y) * glm::cos(m_polar.x),
    glm::sin(m_polar.y) * glm::sin(m_polar.x),
    glm::cos(m_polar.y)
  );
  m_up = glm::vec3(m_forward.z, m_forward.y, -m_forward.x);
  m_right = glm::cross(m_up, m_forward);

  return glm::lookAt(m_position + m_forward * m_distance, m_position, m_up);
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

// ------------------------------------------------------------------------------------------------
Physical::Physical()
  : Meshable(), m_rendererRef(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
Physical::~Physical()
{
  // TODO: remove Physical on destruction
  // 
  //m_rendererRef->getCollisionManager()->removePhysical(this);
}

// ------------------------------------------------------------------------------------------------
void Physical::beforeInitialize(Renderer* renderer)
{
  m_rendererRef = renderer;

  renderer->getCollisionManager()->addPhysical(this);
}

// ------------------------------------------------------------------------------------------------
Box::Box(glm::vec3 scale) : Physical(), m_scale(scale) {}

// ------------------------------------------------------------------------------------------------
glm::vec3 Box::getScale() const
{
  return m_scale;
}

// ------------------------------------------------------------------------------------------------
void Box::computeCollision(CollisionManager* colMan)
{
  // TODO
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
}

// ------------------------------------------------------------------------------------------------
void Box::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.worldToLocal,
                             6 * // Face on Cube
                             2); // Triangle per Face
}

// ------------------------------------------------------------------------------------------------
RigidBody::RigidBody(const std::shared_ptr<Meshable>& target, double mass)
  : m_target(target), m_mass(mass),
  m_position(glm::vec3(0.0)), m_rotation(glm::mat3(1.0)),
  m_linear_velocity(glm::vec3(0.0)), m_derived_rotation(glm::mat3(0.0)),
  m_force(glm::vec3(0.0)), m_torque(glm::vec3(0.0))
{
  if (target == nullptr)
  {
    return;
  }

  addChild(target);

  // Compute Inertia Matrix
  target->initialize(nullptr);
  computeInertia();
}

// ------------------------------------------------------------------------------------------------
size_t RigidBody::addForce(const ExternalForce& force)
{
  m_external_forces.push_back(force);
  computeForceTorque();

  return m_external_forces.size();
}

// ------------------------------------------------------------------------------------------------
bool RigidBody::removeForce(size_t index)
{
  if (index >= m_external_forces.size())
  {
    return false;
  }

  m_external_forces.erase(m_external_forces.begin() + index);
  computeForceTorque();

  return true;
}

// ------------------------------------------------------------------------------------------------
bool RigidBody::setMass(double mass)
{
  if (mass <= 0.0)
  {
    return false;
  }

  m_mass = mass;
  computeInertia();

  return true;
}

// ------------------------------------------------------------------------------------------------
double RigidBody::getMass() const
{
  return m_mass;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  // Position
  m_linear_velocity += m_force * (float) (data.dt / m_mass);
  m_position += m_linear_velocity * (float) data.dt;

  // Rotation
  glm::mat3 Rt = glm::transpose(m_rotation);
  glm::mat3 invI = m_rotation * m_invIBody * Rt;
  glm::vec3 angular_velocity = invI * m_torque;
  // ---
  auto starMatrix = [](const glm::vec3& v) -> glm::mat3
  {
    return glm::mat3(0.0, -v.z, v.y, v.z, 0.0, -v.x, -v.y, v.x, 0.0);
  };
  m_derived_rotation = starMatrix(angular_velocity) * m_rotation;
  // ---
  m_rotation += m_derived_rotation * (float) data.dt; // TODO ?

  updateTransform();
  data.worldToLocal = data.worldToParent * m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::updateTransform()
{
  glm::mat4x4 mat(m_rotation);
  mat = glm::translate(mat, m_position);

  setLocalModel(mat);
}

// ------------------------------------------------------------------------------------------------
void RigidBody::computeForceTorque()
{
  auto forceAcc = [](glm::vec3 r, ExternalForce f)
  {
    return r + f.force;
  };
  m_force = std::accumulate(m_external_forces.begin(), m_external_forces.end(), glm::vec3(0.0f), forceAcc);

  auto torqueAcc = [](glm::vec3 r, ExternalForce f)
  {
    return r + glm::cross(f.position, f.force);
  };
  m_torque = std::accumulate(m_external_forces.begin(), m_external_forces.end(), glm::vec3(0.0f), torqueAcc);
}

// ------------------------------------------------------------------------------------------------
void RigidBody::computeInertia()
{
  auto vertices = m_target->getVertices();
  float pmass = (float) m_mass / vertices.size();

  glm::mat3 body_inertia(0.0f);
  for (const auto& vertex : vertices)
  {
    glm::mat3 m = glm::outerProduct(vertex.position, vertex.position);
    body_inertia += glm::dot(vertex.position, vertex.position) * glm::mat3() - m;
  }

  m_invIBody = glm::inverse(body_inertia * pmass);
}
