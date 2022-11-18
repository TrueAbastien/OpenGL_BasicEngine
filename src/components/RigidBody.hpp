#pragma once

#include "Component.hpp"

class RigidBody final : public Component
{
public:
  friend CollisionSolver;

public:
  struct ExternalForce
  {
    glm::vec3 position;
    glm::vec3 force;
  };

public:
  RigidBody(const std::shared_ptr<Physical>& target,
            float mass = 1.0f,
            float elasticity = 0.0f,
            bool isKinematic = false,
            bool useGravity = false);

  // Returns the current amount of External Forces
  size_t addForce(const ExternalForce& force);
  size_t addForces(const std::vector<ExternalForce>& forces);
  bool removeForce(size_t index);

  bool setMass(float mass);
  float getMass() const;

  bool setElasticity(float elasticity);
  float getElasticity() const;

  void setKinematicState(bool isKinematic);
  bool isKinematic() const;

  void setGravityUse(bool useGravity);
  bool useGravity() const;

  void translateBy(const glm::vec3& trsl);
  void rotateBy(const glm::vec3& rot);

protected:
  void initialize(Renderer* renderer) override;

  glm::mat3 getInvI() const;

private:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

  void updateTransform();
  void computeForceTorque();
  void computeInertia();

protected:
  std::shared_ptr<Physical> m_target;

  glm::vec3 m_currLinearVelocity;
  glm::vec3 m_currAngularMomentum;

  glm::vec3 m_nextLinearVelocity;
  glm::vec3 m_nextAngularMomentum;

  glm::mat3 m_invIBody;

  glm::vec3 m_position;
  glm::vec3 m_rotation;

  float m_mass;
  float m_elasticity;
  bool m_isKinematic;
  bool m_useGravity;

  std::vector<ExternalForce> m_external_forces;

  glm::vec3 m_force;
  glm::vec3 m_torque;
};
