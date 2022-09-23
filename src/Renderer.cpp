#include "Renderer.hpp"
#include "asset.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>

// ------------------------------------------------------------------------------------------------
Renderer::Renderer()
{
}

// ------------------------------------------------------------------------------------------------
void Renderer::start(Component* scene)
{
  scene->initialize(this);
}

// ------------------------------------------------------------------------------------------------
void Renderer::update(Component* scene, UpdateData data)
{
  scene->update(this, data);
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Renderer::getProjection() const
{
  return projection;
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Renderer::getView() const
{
  return view;
}

// ------------------------------------------------------------------------------------------------
void Renderer::setProjection(const glm::mat4& tr)
{
  projection = tr;
}

// ------------------------------------------------------------------------------------------------
void Renderer::setView(const glm::mat4& tr)
{
  view = tr;
}
