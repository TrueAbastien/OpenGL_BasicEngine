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
  auto reflect = [](glm::vec3 v, glm::vec3 n)
  {
    return 2.0f * glm::dot(-v, n) * n + v;
  };

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
      auto body1 = target->getRigidBody();
      auto body2 = result.first->getRigidBody();

      if (body1->isKinematic())
      {
        continue;
      }

      glm::vec3 r1 =
        glm::inverse(body1->localToWorld())
        * glm::vec4(result.second.first.worldPosition, 1.0);
      glm::vec3 r2 =
        glm::inverse(body2->localToWorld())
        * glm::vec4(result.second.second.worldPosition, 1.0);

      /*float target_velocity = glm::dot(
        target_body->m_currLinearVelocity,
        result.second.first.normal);
      float other_velocity = glm::dot(
        other_body->m_currLinearVelocity,
        result.second.second.normal);

      float velocity = (
        target_velocity * target_body->m_mass +
        other_velocity * other_body->m_mass +
        (other_velocity - target_velocity) * other_body->m_mass * target_body->m_elasticity) /
        (target_body->m_mass + other_body->m_mass);

      target_body->m_nextLinearVelocity = result.second.first.normal * velocity;

      target_body->m_nextAngularMomentum = glm::cross(inTarget_localPosition, result.second.first.normal) * velocity;*/

      /*glm::vec3 vel1 = body1->m_currLinearVelocity +
        glm::cross(body1->m_currAngularMomentum, r1);
      glm::vec3 vel2 = body2->m_currLinearVelocity +
        glm::cross(body2->m_currAngularMomentum, r2);
      glm::vec3 dv = vel1 - vel2;*/

      glm::vec3 v1 = body1->m_currLinearVelocity;
      glm::vec3 w1 = body1->m_currAngularMomentum;
      glm::vec3 v2 = body2->m_currLinearVelocity;
      glm::vec3 w2 = body2->m_currAngularMomentum;
      glm::vec3 n = result.second.first.normal;

      glm::mat3 invI1 = body1->getInvI();
      glm::mat3 invI2 = body2->getInvI();

      float m1 = body1->m_mass;
      float m2 = body2->m_mass;

      float impulse = (1 + body1->m_elasticity) *
        (glm::dot(v1 - v2, n) + glm::dot(glm::cross(r1, n), w1) - glm::dot(glm::cross(r2, n), w2)) /
        ((1 / m1) + (1 / m2) +
         glm::dot(glm::cross(r1, n), invI1 * glm::cross(r1, n)) +
         glm::dot(glm::cross(r2, n), invI2 * glm::cross(r2, n)));
      glm::vec3 j = impulse * n;
      
      body1->m_nextLinearVelocity -= j / m1;
      body2->m_nextAngularMomentum -= invI1 * glm::cross(j, r1);
    }
  }
}
