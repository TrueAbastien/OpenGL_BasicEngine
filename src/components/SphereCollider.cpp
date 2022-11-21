#include "SphereCollider.hpp"

#include "CollisionManager.hpp"
#include "Renderable.hpp"

#include <glm/gtc/matrix_transform.hpp>

// ------------------------------------------------------------------------------------------------
template <typename TComponent>
std::optional<std::pair<float, glm::vec3>> getBoundsInfo(const std::shared_ptr<TComponent>& comp)
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

  glm::vec3 bounds = (max - min) * 0.5f;
  float radius = std::max({bounds.x, bounds.y, bounds.z});
  glm::vec3 offset = max - bounds;

  return std::make_pair(radius, offset);
}

// ------------------------------------------------------------------------------------------------
SphereCollider::SphereCollider(const std::shared_ptr<Meshable>& target)
  : Physical(target), WFSphereBuilder(0.0f)
{
  const auto infos = getBoundsInfo(target);
  if (!infos.has_value())
  {
    return;
  }

  m_radius = infos->first;
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
SphereCollider::SphereCollider(const std::shared_ptr<TexturedMesh>& mesh)
  : Physical(mesh), WFSphereBuilder(0.0f)
{
  const auto infos = getBoundsInfo(mesh);
  if (!infos.has_value())
  {
    return;
  }

  m_radius = infos->first;
  m_localToParent = glm::translate(mesh->getLocalToParent(), infos->second);
  mesh->setLocalToParent(glm::translate(glm::mat4(1.0), -infos->second));

  Meshable::makeMesh(makeMeshContent(
    [](auto)
    {
      return glm::vec4(0.0, 1.0, 0.0, 0.1);
    }
  ));
}

// ------------------------------------------------------------------------------------------------
CurrentTargetCollisions SphereCollider::computeCollision(CollisionManager* colMan)
{
  return colMan->computeTargetCollisions(this);
}

// ------------------------------------------------------------------------------------------------
void SphereCollider::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();

  Physical::beforeInitialize(renderer);
}

// ------------------------------------------------------------------------------------------------
void SphereCollider::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             20 *     // Faces on icosahedron
                             4 * 4 *  // Divisions
                             3 *
                             2);      // Values
}
