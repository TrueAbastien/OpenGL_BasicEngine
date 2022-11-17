#include "SphereCollider.hpp"

#include "CollisionManager.hpp"
#include "Renderable.hpp"

#include <glm/gtc/matrix_transform.hpp>

SphereCollider::SphereCollider(const std::shared_ptr<Meshable>& target)
  : Physical(target), WFSphereBuilder(0.0f)
{
  if (target == nullptr)
  {
    return;
  }

  const auto vertices = target->getVertices();
  if (vertices.empty())
  {
    return;
  }

  float min = vertices[0].position.x, max = vertices[0].position.x;

  size_t size = vertices.size();
  for (size_t index = 1; index < size; ++index)
  {
    glm::vec3 pos = vertices[index].position;

    if      (pos.x < min) min = pos.x;
    else if (pos.x > max) max = pos.x;
  }

  m_radius = (max - min) * 0.5f;
<<<<<<< HEAD
  // float offset = max - m_radius * 0.5f;

  // m_localToParent = glm::translate(target->getLocalToParent(), glm::vec3(offset));
=======
  float offset = max - m_radius * 0.5f;

  m_localToParent = glm::translate(target->getLocalToParent(), glm::vec3(offset));
>>>>>>> f8229fc... WFSphereBuilder and SphereCollider. No internalCompute algorithm yet
  target->setLocalToParent(glm::mat4(1.0));


  Meshable::makeMesh(makeMeshContent(
    [](auto)
    {
      return glm::vec4(0.0, 1.0, 0.0, 0.1);
    }
  ));
}

CurrentTargetCollisions SphereCollider::computeCollision(CollisionManager* colMan)
{
  return colMan->computeTargetCollisions(this);
}

void SphereCollider::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();

  Physical::beforeInitialize(renderer);
}

void SphereCollider::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             20 *     // Faces on icosahedron
                             4 * 4 *  // Divisions
                             3 *
                             2);      // Values
}
