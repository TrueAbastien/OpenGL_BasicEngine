#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <concepts>
#include <iostream>

#include "Shader.hpp"
#include "glError.hpp"
#include "StructInfo.hpp"

class Component
{
public:
  using Pointer = std::shared_ptr<Component>;
  using Collection = std::vector<Pointer>;
  using UPointer = std::unique_ptr<Component>;

public:
  void setLocalToParent(const glm::mat4& model);
  void setLocalToParent(const glm::vec3& trsl = glm::vec3(0.0),
                        const glm::vec3& rot = glm::vec3(0.0));
  glm::mat4 getLocalToParent() const;

  bool addChild(const Pointer& child);
  bool removeChild(const Pointer& child);
  bool removeChild(int index);
  void removeChildren();
  Pointer getChild(int index) const;
  Collection getChildren() const;
  bool containsChild(const Pointer& child) const;

  void setParent(Component* parent);
  glm::mat4 localToWorld() const;

public:
  virtual void initialize(Renderer* renderer);
  virtual void update(Renderer* renderer, UpdateData& data);

protected:
  virtual void beforeInitialize(Renderer* renderer) {}
  virtual void beforeUpdate(Renderer* renderer, UpdateData& data) {}

protected:
  Component() = default;

protected:
  glm::mat4 m_localToParent;
  Collection m_children;

  Component* m_parent = nullptr;
};
