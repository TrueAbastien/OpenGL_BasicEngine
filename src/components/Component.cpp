#include "Component.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "asset.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

// ------------------------------------------------------------------------------------------------
void Component::setLocalModel(const glm::mat4& model)
{
  m_parentToLocal = model;
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalModel(const glm::vec3& trsl, const glm::vec3& rot)
{
  glm::mat4 tr = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0)) *
                 glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0)) *
                 glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
  m_parentToLocal = glm::translate(tr, trsl);
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Component::getLocalModel() const
{
  return m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
bool Component::addChild(const Pointer& child)
{
  if (containsChild(child))
  {
    return false;
  }

  child->setParent(this);

  m_children.push_back(child);
  return true;
}

// ------------------------------------------------------------------------------------------------
bool Component::removeChild(const Pointer& child)
{
  return std::erase(m_children, child) > 0;
}

// ------------------------------------------------------------------------------------------------
bool Component::removeChild(int index)
{
  if (index < 0 || index >= m_children.size())
  {
    return false;
  }

  m_children.erase(m_children.begin() + index);
  return true;
}

// ------------------------------------------------------------------------------------------------
Component::Pointer Component::getChild(int index) const
{
  if (index < 0 || index >= m_children.size())
  {
    return nullptr;
  }

  return m_children.at(index);
}

// ------------------------------------------------------------------------------------------------
Component::Collection Component::getChildren() const
{
  return m_children;
}

// ------------------------------------------------------------------------------------------------
bool Component::containsChild(const Pointer& child) const
{
  auto it = std::find(m_children.begin(), m_children.end(), child);

  return it != m_children.end();
}

// ------------------------------------------------------------------------------------------------
void Component::setParent(Component* parent)
{
  m_parent = parent;
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Component::worldToLocal() const
{
  if (m_parent != nullptr)
  {
    return m_parent->worldToLocal() * m_parentToLocal;
  }

  return m_parentToLocal;
}

// ------------------------------------------------------------------------------------------------
void Component::initialize(Renderer* renderer)
{
  beforeInitialize(renderer);

  for (auto& child : m_children)
  {
    child->initialize(renderer);
  }
}

// ------------------------------------------------------------------------------------------------
void Component::update(Renderer* renderer, UpdateData& data)
{
  UpdateData newData = data;
  newData.worldToParent = data.worldToLocal;
  newData.worldToLocal *= m_parentToLocal;

  beforeUpdate(renderer, newData);

  for (auto& child : m_children)
  {
    child->update(renderer, newData);
  }
}
