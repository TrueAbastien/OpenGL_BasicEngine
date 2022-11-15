#include "CollisionSolver.hpp"

#include "RigidBody.hpp"

// ------------------------------------------------------------------------------------------------
CollisionSolver::CollisionSolver(CollisionManager* manager)
  : m_manager(manager)
{
}

// ------------------------------------------------------------------------------------------------
void CollisionSolver::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  auto collisionsMap = m_manager->computeAllCollisions();

  for (auto& collisions : collisionsMap)
  {
    if (collisions.second.empty())
    {
      continue;
    }

    // Only One Collision
    auto target = collisions.first;
    auto result = *(collisions.second.begin());

    if (result.first->hasBody() && target->hasBody())
    {
      auto target_body = target->getRigidBody();
      auto other_body = result.first->getRigidBody();

      if (target_body->isKinematic())
      {
        continue;
      }

      glm::vec3 inTarget_localPosition =
        glm::inverse(target_body->localToWorld())
        * glm::vec4(result.second.first.worldPosition, 1.0);
      glm::vec3 inOther_localPosition =
        glm::inverse(other_body->localToWorld())
        * glm::vec4(result.second.second.worldPosition, 1.0);

      float target_velocity = glm::max(glm::dot(
        target_body->m_currLinearVelocity,
        result.second.first.normal), 0.0f);
      float other_velocity = glm::max(glm::dot(
        other_body->m_currLinearVelocity,
        result.second.second.normal), 0.0f);

      float velocity = (
        target_velocity * target_body->m_mass +
        other_velocity * other_body->m_mass +
        (other_velocity - target_velocity) * other_body->m_mass * target_body->m_elasticity) /
        (target_body->m_mass + other_body->m_mass);

      target_body->m_nextLinearVelocity = result.second.first.normal * velocity;

      target_body->m_nextAngularMomentum = glm::cross(inTarget_localPosition, result.second.first.normal) * velocity;
    }
  }
}
