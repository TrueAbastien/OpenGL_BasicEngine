#include "CollisionDetector.hpp"

#include "RigidBody.hpp"

// ------------------------------------------------------------------------------------------------
CollisionDetector::CollisionDetector(CollisionManager* manager)
  : m_manager(manager)
{
}

// ------------------------------------------------------------------------------------------------
void CollisionDetector::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  auto collisionsMap = m_manager->computeAllCollisions();

  auto removeCollision = [&](Physical* target, Physical* body)
  {
    auto& targetMap = collisionsMap[target];
    auto targetIt = targetMap.find(body);
    targetMap.erase(targetIt);

    auto& bodyMap = collisionsMap[body];
    auto bodyIt = bodyMap.find(target);
    bodyMap.erase(bodyIt);
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

      float rb1_velocity = glm::dot(rb1->m_linear_velocity, result.second.first.normal);
      float rb2_velocity = glm::dot(rb2->m_linear_velocity, result.second.second.normal);

      float v1 = (
        rb1_velocity * rb1->m_mass +
        rb2_velocity * rb2->m_mass +
        (rb2_velocity - rb1_velocity) * rb2->m_mass * rb1->m_elasticity) /
        (rb1->m_mass + rb2->m_mass);

      float v2 = (
        rb1_velocity * rb1->m_mass +
        rb2_velocity * rb2->m_mass +
        (rb1_velocity - rb2_velocity) * rb1->m_mass * rb2->m_elasticity) /
        (rb1->m_mass + rb2->m_mass);

      auto reflect = [&](glm::vec3 dir, glm::vec3 normal, float velocity) -> glm::vec3
      {
        if (dir == glm::vec3(0.0))
        {
          return normal * velocity;
        }

        glm::vec3 d = glm::normalize(dir);
        return (2.0f * glm::dot(normal, -d) * normal + d) * velocity;
      };

      rb1->m_linear_velocity = reflect(rb1->m_linear_velocity, result.second.first.normal, v1);
      rb2->m_linear_velocity = reflect(rb2->m_linear_velocity, result.second.second.normal, v2);
    }

    // TODO: better implementation

    // Remove in-between collisions
    removeCollision(target, result.first);
  }
}