#include "RigidBody.hpp"

#include "Physical.hpp"
#include "Renderer.hpp"

#include <numeric>
#include <algorithm>

// ------------------------------------------------------------------------------------------------
RigidBody::RigidBody(const std::shared_ptr<Physical>& target, double mass)
  : m_target(target), m_mass(mass),
  m_position(glm::vec3(0.0)), m_rotation(glm::vec3(0.0)),
  m_linear_velocity(glm::vec3(0.0)), //m_derived_rotation(glm::mat3(0.0)),
  m_angularMomentum(glm::vec3(0.0)),
  m_force(glm::vec3(0.0)), m_torque(glm::vec3(0.0))
{
  if (target == nullptr)
  {
    return;
  }

  addChild(target);
}

// ------------------------------------------------------------------------------------------------
size_t RigidBody::addForce(const ExternalForce& force)
{
  m_external_forces.push_back(force);
  computeForceTorque();

  return m_external_forces.size();
}

// ------------------------------------------------------------------------------------------------
size_t RigidBody::addForces(const std::vector<ExternalForce>& forces)
{
  m_external_forces.insert(m_external_forces.end(), forces.begin(), forces.end());
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
void RigidBody::translateBy(const glm::vec3& trsl)
{
  m_position += trsl;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::rotateBy(const glm::vec3& rot)
{
  m_rotation += rot;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::initialize(Renderer* renderer)
{
  for (auto& child : m_children)
  {
    child->initialize(renderer);
  }

  // Compute Inertia Matrix
  computeInertia();
}

// ------------------------------------------------------------------------------------------------
void RigidBody::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  removeImpulseForce();

  // Collision(s) response
  auto collisions = m_target->computeCollision(renderer->getCollisionManager().get());
  if (!collisions.empty())
  {
    /*for (const auto& collision : collisions)
    {
      m_position += collision.second.second.worldPosition - collision.second.first.worldPosition;
    }*/

    return; // TEMP

    // TODO
  }

  // Position
  m_linear_velocity += m_force * (float) (data.dt / m_mass);
  m_position += m_linear_velocity * (float) data.dt;

  // Rotation
  m_angularMomentum += m_torque * (float) data.dt;
  // ---
  glm::mat3 rot = m_parentToLocal;
  glm::mat3 rot_t = glm::transpose(rot);
  glm::mat3 invI = rot * m_invIBody * rot_t;
  glm::vec3 angular_velocity = invI * m_angularMomentum;
  // ---
  /*auto starMatrix = [](const glm::vec3& v) -> glm::mat3
  {
    return glm::mat3(
      0.0, -v.z, v.y,
      v.z, 0.0, -v.x,
      -v.y, v.x, 0.0);
  };
  m_derived_rotation = starMatrix(glm::normalize(angular_velocity)) * m_rotation;*/
  // ---
  m_rotation += angular_velocity * (float) data.dt;

  updateTransform();
  data.worldToLocal = data.worldToParent * m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
void RigidBody::updateTransform()
{
  setLocalModel(m_position, m_rotation);
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

// ------------------------------------------------------------------------------------------------
void RigidBody::removeImpulseForce()
{
  auto isImpulse = [](ExternalForce ea)
  {
    return (ea.mode == ForceMode::IMPULSE);
  };
  std::erase_if(m_external_forces, isImpulse);

  // Recomputes Force & Torque
  computeForceTorque();
}
