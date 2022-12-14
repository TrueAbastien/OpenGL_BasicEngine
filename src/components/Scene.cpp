#include "Scene.hpp"

#include "Camera.hpp"
#include "CollisionSolver.hpp"
#include "Renderer.hpp"
#include "World.hpp"

#include "imgui_impl_glfw.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ------------------------------------------------------------------------------------------------
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (mainCamera) mainCamera->mouseMoveCallback(xpos, ypos);

  // ImGui
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

// ------------------------------------------------------------------------------------------------
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
  double xpos, ypos; glfwGetCursorPos(window, &xpos, &ypos);
  if (mainCamera) mainCamera->mouseClickCallback(button, action, xpos, ypos);

  // ImGui
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

// ------------------------------------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (mainCamera) mainCamera->scrollBack((float) yoffset);

  // ImGui
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

// ------------------------------------------------------------------------------------------------
void Scene::construct(Renderer* renderer)
{
  addChild(std::make_shared<Camera>());
  addChild(std::make_shared<CollisionSolver>(renderer->getCollisionManager().get()));

#ifdef _DEBUG
  addChild(std::make_shared<World>());
#endif
}

// ------------------------------------------------------------------------------------------------
void Scene::beforeInitialize(Renderer* renderer)
{
  if (mainCamera != nullptr)
  {
    GLFWwindow* window = renderer->getWindow();

    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetScrollCallback(window, scrollCallback);
  }
}

// ------------------------------------------------------------------------------------------------
void Scene::beforeUpdate(Renderer* renderer, UpdateData& data)
{
}
