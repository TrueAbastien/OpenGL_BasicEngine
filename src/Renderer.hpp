#pragma once

#include "Application.hpp"
#include "Shader.hpp"
#include "CollisionManager.hpp"

#include "components/Component.hpp"

class Renderer final
{
public:
  Renderer(GLFWwindow* window);

public:
  void start(Component* scene);
  void update(Component* scene, UpdateData data);

  glm::mat4 getProjection() const;
  glm::mat4 getView() const;

  void setProjection(const glm::mat4& tr);
  void setView(const glm::mat4& tr);

  GLFWwindow* getWindow() const;

  std::shared_ptr<CollisionManager> getCollisionManager() const;

private:
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);

  GLFWwindow* window;

  std::shared_ptr<CollisionManager> collisionManager;
};