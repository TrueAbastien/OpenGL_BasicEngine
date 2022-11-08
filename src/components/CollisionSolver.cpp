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

  /*auto removeCollision = [&](Physical* target, Physical* body)
  {
    auto& targetMap = collisionsMap[target];
    auto targetIt = targetMap.find(body);
    targetMap.erase(targetIt);

    auto& bodyMap = collisionsMap[body];
    auto bodyIt = bodyMap.find(target);
    bodyMap.erase(bodyIt);
  };*/

  auto reflect = [](glm::vec3 dir, glm::vec3 normal, float velocity) -> glm::vec3
  {
    if (dir == glm::vec3(0.0))
    {
      return normal * velocity;
    }

    glm::vec3 d = glm::normalize(dir);
    return (2.0f * glm::dot(normal, -d) * normal + d) * velocity;
  };

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
      auto rb1 = target->getRigidBody();
      auto rb2 = result.first->getRigidBody();

      if (rb1->isKinematic())
      {
        continue;
      }

      glm::vec3 localPosition = glm::inverse(rb1->localToWorld()) * glm::vec4(result.second.first.worldPosition, 1.0);

      float rb1_velocity = glm::dot(rb1->m_currLinearVelocity, result.second.first.normal);
      float rb2_velocity = glm::dot(rb2->m_currLinearVelocity, result.second.second.normal);

      float v1 = (
        rb1_velocity * rb1->m_mass +
        rb2_velocity * rb2->m_mass +
        (rb2_velocity - rb1_velocity) * rb2->m_mass * rb1->m_elasticity) /
        (rb1->m_mass + rb2->m_mass);

      /*float v2 = (
        rb1_velocity * rb1->m_mass +
        rb2_velocity * rb2->m_mass +
        (rb1_velocity - rb2_velocity) * rb1->m_mass * rb2->m_elasticity) /
        (rb1->m_mass + rb2->m_mass);*/

      rb1->m_nextLinearVelocity = reflect(rb1->m_currLinearVelocity, result.second.first.normal, v1);
      //rb2->m_nextLinearVelocity = reflect(rb2->m_currLinearVelocity, result.second.second.normal, v2);

      rb1->m_nextAngularMomentum = glm::cross(localPosition, result.second.first.normal);
    }

    // TODO: better implementation

    // Remove in-between collisions
    //removeCollision(target, result.first);
  }
}
