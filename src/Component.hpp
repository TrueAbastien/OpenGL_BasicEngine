
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
class CollisionManager;
class Physical;

// Forward Inner Using (see. CollisionManager.hpp)
using CurrentTargetCollisions = std::map<Physical*, std::pair<glm::vec3, glm::vec3>>;

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

  bool addChild(const Pointer& child);
  bool removeChild(const Pointer& child);
  bool removeChild(int index);
  Pointer getChild(int index) const;
  Collection getChildren() const;
  bool containsChild(const Pointer& child) const;

  void setParent(Component* parent);
  glm::mat4 worldToLocal() const;

public:
  virtual void initialize(Renderer* renderer);
  virtual void update(Renderer* renderer, UpdateData& data);

protected:
  virtual void beforeInitialize(Renderer* renderer) {}
  virtual void beforeUpdate(Renderer* renderer, UpdateData& data) {}

protected:
  Component() = default;

protected:
  glm::mat4 m_parentToLocal;
  Collection m_children;

  Component* m_parent = nullptr;
};

// ------------------------------------------------------------------------------------------------
class Scene final : public Component
{
public:
  Scene();

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
};

// ------------------------------------------------------------------------------------------------
class EmptyTransform final : public Component
{
public:
  EmptyTransform() = default;
};

// ------------------------------------------------------------------------------------------------
class Camera final : public Component
{
public:
  Camera();

public:
  void processInput(unsigned char press, float dt);
  void mouseMoveCallback(double xpos, double ypos);
  void mouseClickCallback(int button, int action, double xpos, double ypos);
  void scrollBack(float yoffset);

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  glm::mat4 computeView();

private:
  glm::vec3 m_position;
  glm::vec2 m_polar; // Theta - Phi
  float m_distance;

  glm::vec3 m_up;
  glm::vec3 m_forward;
  glm::vec3 m_right;

  bool m_enableRotation;
  double m_lastX;
  double m_lastY;
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
class VisibleTransform final : public Renderable
{
public:
  VisibleTransform();

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;
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

  std::vector<VertexType> getVertices() const;
  std::vector<GLuint> getIndexes() const;

protected:
  std::vector<VertexType> m_vertices;
  std::vector<GLuint> m_indexes;
};

// ------------------------------------------------------------------------------------------------
class Physical : public Meshable
{
protected:
  Physical();

public:
  virtual ~Physical();

protected:
  virtual void beforeInitialize(Renderer* renderer) override;

public:
  virtual CurrentTargetCollisions computeCollision(CollisionManager* colMan) = 0;

private:
  Renderer* m_rendererRef;
};

// ------------------------------------------------------------------------------------------------
class Box : public Physical
{
public:
  Box(glm::vec3 scale);

  glm::vec3 getScale() const;

  CurrentTargetCollisions computeCollision(CollisionManager* colMan) override;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

protected:
  glm::vec3 m_scale;
};

// ------------------------------------------------------------------------------------------------
class RigidBody final : public Component
{
public:
  struct ExternalForce
  {
    glm::vec3 position;
    glm::vec3 force;
  };

public:
  RigidBody(const std::shared_ptr<Physical>& target, double mass = 1.0);

  // Returns the current amount of External Forces
  size_t addForce(const ExternalForce& force);
  bool removeForce(size_t index);

  bool setMass(double mass);
  double getMass() const;

protected:
  void initialize(Renderer* renderer) override;

private:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

  void updateTransform();
  void computeForceTorque();
  void computeInertia();

protected:
  std::shared_ptr<Physical> m_target;

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
