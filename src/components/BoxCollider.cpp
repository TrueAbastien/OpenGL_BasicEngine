#include "BoxCollider.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

// ------------------------------------------------------------------------------------------------
template <typename TComponent>
std::optional<std::pair<glm::vec3, glm::vec3>> getBoundsInfo(const std::shared_ptr<TComponent>& comp)
{
  if (comp == nullptr)
  {
    return std::nullopt;
  }

  const auto vertices = comp->getVertices();
  if (vertices.empty())
  {
    return std::nullopt;
  }

  glm::vec3 min = vertices[0].position, max = vertices[0].position;

  size_t size = vertices.size();
  for (size_t index = 1; index < size; ++index)
  {
    glm::vec3 pos = vertices[index].position;

    if (pos.x < min.x) min.x = pos.x;
    else if (pos.x > max.x) max.x = pos.x;

    if (pos.y < min.y) min.y = pos.y;
    else if (pos.y > max.y) max.y = pos.y;

    if (pos.z < min.z) min.z = pos.z;
    else if (pos.z > max.z) max.z = pos.z;
  }

  glm::vec3 scale = max - min;
  glm::vec3 offset = max - scale * 0.5f;

  return std::make_pair(scale, offset);
}

// ------------------------------------------------------------------------------------------------
BoxCollider::BoxCollider(const std::shared_ptr<Meshable>& target)
  : Physical(target), WFBoxBuilder(glm::vec3(0.0)), OBBSeparatingAxis()
{
  const auto infos = getBoundsInfo(target);
  if (!infos.has_value())
  {
    return;
  }

  m_scale = infos->first;
  m_localToParent = glm::translate(target->getLocalToParent(), infos->second);
  target->setLocalToParent(glm::translate(glm::mat4(1.0), -infos->second));

  Meshable::makeMesh(makeMeshContent(
    [](auto)
    {
      return glm::vec4(0.0, 1.0, 0.0, 0.1);
    }
  ));
}

// ------------------------------------------------------------------------------------------------
BoxCollider::BoxCollider(const std::shared_ptr<TexturedMesh>& mesh)
  : Physical(mesh), WFBoxBuilder(glm::vec3(0.0)), OBBSeparatingAxis()
{
  const auto infos = getBoundsInfo(mesh);
  if (!infos.has_value())
  {
    return;
  }

  m_scale = infos->first;
  m_localToParent = glm::translate(mesh->getLocalToParent(), infos->second);
  mesh->setLocalToParent(glm::translate(glm::mat4(1.0), -infos->second));

#ifdef _DEBUG
  Meshable::makeMesh(makeMeshContent(
    [](auto)
    {
      return glm::vec4(0.0, 1.0, 0.0, 0.1);
    }
  ));
#endif
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
#ifdef _DEBUG
  Meshable::updateRenderable(renderer, data.localToWorld,
                             12 * // Lines on Cube
                             2);  // Values amount
#endif
}
