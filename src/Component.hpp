
#pragma once

#include "Shader.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <concepts>
#include <iostream>

#include "glError.hpp"

// Forward Declaration
class Renderer;

// Vertex Data Content
struct VertexType {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

// Update Data
struct UpdateData
{
  glm::mat4 worldToParent;
  glm::mat4 worldToLocal;
  double dt;
  double t;
};

// ------------------------------------------------------------------------------------------------
class Component
{
 public:
   using Pointer = std::shared_ptr<Component>;
   using Collection = std::vector<Pointer>;
   using UPointer = std::unique_ptr<Component>;

 public:
  void setLocalModel(const glm::mat4& model);
  void setLocalModel(const glm::vec3& trsl = glm::vec3(),
                     const glm::vec3& rot = glm::vec3());
  glm::mat4 getLocalModel() const;

  // Returns index
  bool addChild(const Pointer& child);
  bool removeChild(const Pointer& child);
  bool removeChild(int index);
  Pointer getChild(int index) const;
  Collection getChildren() const;
  bool containsChild(const Pointer& child) const;

public:
  void initialize(Renderer* renderer);
  void update(Renderer* renderer, UpdateData& data);

protected:
  virtual void beforeInitialize(Renderer* renderer) {}
  virtual void beforeUpdate(Renderer* renderer, UpdateData& data) {}

protected:
  Component() = default;

protected:
  glm::mat4 m_parentToLocal;
  Collection m_children;
};

// ------------------------------------------------------------------------------------------------
class Scene final : public Component
{
public:
  Scene() = default;
};

// ------------------------------------------------------------------------------------------------
class EmptyTransform final : public Component
{
public:
  EmptyTransform() = default;
};

// ------------------------------------------------------------------------------------------------
class Renderable : public Component
{
protected:
  Renderable();

protected:
  virtual void initializeRenderable(std::vector<VertexType> vertices, std::vector<GLuint> index);
  virtual void updateRenderable(Renderer* renderer, glm::mat4 worldToLocal, GLsizei nFaces);

protected:
  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;
};

// ------------------------------------------------------------------------------------------------
class Meshable : public Renderable
{
protected:
  Meshable();

protected:
  void initializeRenderable(std::vector<VertexType> vertices, std::vector<GLuint> index) override;

public:
  void updateMesh();

protected:
  std::vector<VertexType> m_vertices;
  std::vector<GLuint> m_indexes;
};

// ------------------------------------------------------------------------------------------------
class Box : public Renderable
{
public:
  Box(glm::vec3 scale);

  glm::vec3 scale() const;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

protected:
  glm::vec3 m_scale;
};

// ------------------------------------------------------------------------------------------------
template <typename T>
concept RenderableConcept = std::derived_from<T, Renderable>;

// Forward Declaration
class RigidBody;

namespace RigidBodyUtilities
{
  template <RenderableConcept T>
  inline glm::mat3 inertiaMoment(RigidBody* rigidBody, const std::shared_ptr<T>& target)
  {
    std::cout << "[Error!] Unspecified for this Renderable, try to specify it in RigidBodyUtilities." << std::endl;

    assert(0);
    return {};
  }
}

class RigidBody final : public Component
{
public:
  template <RenderableConcept T>
  friend glm::mat3x3 RigidBodyUtilities::inertiaMoment(RigidBody* rigidBody, const std::shared_ptr<T>& target);

  struct ExternalForce
  {
    glm::vec3 position;
    glm::vec3 force;
  };

public:
  template <RenderableConcept T>
  RigidBody(const std::shared_ptr<T>& target)
  {
    if (target == nullptr)
    {
      return;
    }

    addChild(target);

    // Compute Inertia Moment Matrix
    m_invIBody = glm::inverse(RigidBodyUtilities::inertiaMoment(target));
  }

  // Returns the current amount of External Forces
  size_t addForce(const ExternalForce& force);
  bool removeForce(size_t index);

  bool setMass(double mass);
  double getMass() const;

private:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

  void updateTransform();
  void computeForceTorque();

protected:
  glm::vec3 m_linear_velocity;
  glm::mat3 m_derived_rotation;

  glm::mat3 m_invIBody;

  glm::vec3 m_position;
  glm::mat3 m_rotation;

  double m_mass;

  std::vector<ExternalForce> m_external_forces;

  glm::vec3 m_force;
  glm::vec3 m_torque;
};

namespace RigidBodyUtilities
{
  template <>
  inline glm::mat3 inertiaMoment<Box>(RigidBody* rigidBody, const std::shared_ptr<Box>& target)
  {
    glm::vec3 s = target->scale() * target->scale();
    glm::vec3 vec(s.y + s.z, s.z + s.x, s.x + s.y);
    vec *= (float) rigidBody->m_mass / 12.0f;
    return glm::mat3(vec.x, 0.0, 0.0, 0.0, vec.y, 0.0, 0.0, 0.0, vec.z);
  }
}