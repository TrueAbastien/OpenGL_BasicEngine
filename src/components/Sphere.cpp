#include "Sphere.hpp"

#include "CollisionManager.hpp"
#include "Renderer.hpp"

Sphere::Sphere(float radius)
  : Meshable(), SphereBuilder(radius)
{
  Meshable::makeMesh(makeMeshContent(
    [](VertexType vt)
    {
      return glm::vec4(
        vt.position.x < 0 ? 1.0 : 0.5,
        vt.position.y < 0 ? 1.0 : 0.5,
        vt.position.z < 0 ? 1.0 : 0.5,
        1.0);
    }
  ));
}

void Sphere::beforeInitialize(Renderer* renderer)
{
  Meshable::initializeMesh();
}

void Sphere::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  Meshable::updateRenderable(renderer, data.localToWorld,
                             20 *     // Faces on Icosahedron
                             4 * 4 *  // Divisions
                             3);      // Values amount
}