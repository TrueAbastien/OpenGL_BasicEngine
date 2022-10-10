#pragma once

#include "Component.hpp"

class RigidBody final : public Component
{
public:
  enum ForceMode
  {
    CONTINUOUS,
    IMPULSE
  };
  struct ExternalForce
  {
    glm::vec3 position;
    glm::vec3 force;
    ForceMode mode = ForceMode::CONTINUOUS;
  };

public:
  RigidBody(const std::shared_ptr<Physical>& target, double mass = 1.0);

  // Returns the current amount of External Forces
  size_t addForce(const ExternalForce& force);
  size_t addForces(const std::vector<ExternalForce>& forces);
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
  void removeImpulseForce();

protected:
  std::shared_ptr<Physical> m_target;

  glm::vec3 m_linear_velocity;
  //glm::mat3 m_derived_rotation;
  glm::vec3 m_angularMomentum;

  glm::mat3 m_invIBody;

  glm::vec3 m_position;
  glm::vec3 m_rotation;
  //glm::mat3 m_rotation;

  double m_mass;

  std::vector<ExternalForce> m_external_forces;

  glm::vec3 m_force;
  glm::vec3 m_torque;
};
