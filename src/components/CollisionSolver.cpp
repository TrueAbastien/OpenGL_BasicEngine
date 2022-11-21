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

    auto target = collisions.first;
    if (!target->hasBody())
    {
      continue;
    }

    auto body1 = target->getRigidBody();
    if (body1->isKinematic())
    {
      continue;
    }

    for (const auto& result : collisions.second)
    {
      if (!result.first->hasBody())
      {
        continue;
      }

      auto body2 = result.first->getRigidBody();

      glm::vec3 r1 = result.second.first.worldPosition - glm::vec3(body1->localToWorld()[3]);
      glm::vec3 r2 = result.second.second.worldPosition - glm::vec3(body2->localToWorld()[3]);

      glm::mat3 invI1 = body1->getInvI();
      glm::mat3 invI2 = body2->getInvI();

      glm::vec3 v1 = body1->m_currLinearVelocity;
      glm::vec3 w1 = body1->m_currAngularMomentum;
      glm::vec3 v2 = body2->m_currLinearVelocity;
      glm::vec3 w2 = body2->m_currAngularMomentum;
      glm::vec3 n = glm::normalize(result.second.first.normal);

      float m1 = body1->m_mass;
      float m2 = body2->m_mass;

      float impulse = (1 + body1->m_elasticity) *
        (glm::dot(v1 - v2, n) + glm::dot(glm::cross(r1, n), w1) - glm::dot(glm::cross(r2, n), w2)) /
        ((1 / m1) + (1 / m2) +
          glm::dot(glm::cross(r1, n), invI1 * glm::cross(r1, n)) +
          glm::dot(glm::cross(r2, n), invI2 * glm::cross(r2, n)));
      glm::vec3 j = impulse * n;

      body1->m_position += n * result.second.first.penetration;
      body1->m_nextLinearVelocity -= j / m1;
      body1->m_nextAngularMomentum -= /*invI1 **/ glm::cross(j, r1);
    }
  }
}
