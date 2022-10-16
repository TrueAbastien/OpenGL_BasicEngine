#include "BoxCollider.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

// ------------------------------------------------------------------------------------------------
BoxCollider::BoxCollider(const std::shared_ptr<Meshable>& target)
  : Physical(target), WFBoxBuilder(glm::vec3(0.0))
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

  glm::vec3 min = vertices[0].position, max = vertices[0].position;

  size_t size = vertices.size();
  for (size_t index = 1; index < size; ++index)
  {
    glm::vec3 pos = vertices[index].position;

    if      (pos.x < min.x) min.x = pos.x;
    else if (pos.x > max.x) max.x = pos.x;

    if      (pos.y < min.y) min.y = pos.y;
    else if (pos.y > max.y) max.y = pos.y;

    if      (pos.z < min.z) min.z = pos.z;
    else if (pos.z > max.z) max.z = pos.z;
  }

  m_scale = max - min;
  glm::vec3 offset = max - m_scale * 0.5f;

  m_localToParent = glm::translate(target->getLocalToParent(), offset);
  target->setLocalToParent(glm::mat4(1.0));

  Meshable::makeMesh(makeMeshContent(
    [](auto)
    {
      return glm::vec4(0.0, 1.0, 0.0, 0.1);
    }
  ));
}

// ------------------------------------------------------------------------------------------------
CurrentTargetCollisions BoxCollider::computeCollision(CollisionManager* colMan)
{
  return colMan->computeTargetCollisions(this);
}

// ------------------------------------------------------------------------------------------------
void BoxCollider::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();

  Physical::beforeInitialize(renderer);
}

// ------------------------------------------------------------------------------------------------
void BoxCollider::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             12 * // Lines on Cube
                             2);  // Values amount
}
