#include "MainApplication.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>

#include "asset.hpp"
#include "glError.hpp"

#include "components/RigidBody.hpp"
#include "components/Box.hpp"
#include "components/Sphere.hpp"
#include "components/TexturedMesh.hpp"

#include "scenes/BowlingScene.hpp"
#include "scenes/BowlsScene.hpp"
#include "scenes/PoolScene.hpp"
#include "scenes/BallpitScene.hpp"
#include "scenes/SpinningBatScene.hpp"
#include "scenes/DominoScene.hpp"
#include "scenes/DynamicScene.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

MainApplication::MainApplication()
    : Application(), m_currentSceneIndex(0), m_scenes(0), m_isPaused(false)
{
  m_renderer = std::make_unique<Renderer>(window);

  glCheckError(__FILE__, __LINE__);

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();

  // Create Scenes
  m_scenes.push_back(std::make_unique<BowlingScene>());
  m_scenes.push_back(std::make_unique<BowlsScene>());
  m_scenes.push_back(std::make_unique<PoolScene>());
  m_scenes.push_back(std::make_unique<BallpitScene>());
  m_scenes.push_back(std::make_unique<SpinningBatScene>());
  m_scenes.push_back(std::make_unique<DominoScene>());
  m_scenes.push_back(std::make_unique<DynamicScene>());
  // TO EXPAND
}

MainApplication::~MainApplication()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void MainApplication::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  // set matrix : projection + view
  m_renderer->setProjection(glm::perspective(
      float(2.0 * atan(getHeight() / 1920.f)), getWindowRatio(), 0.1f, 100.f));

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ImGui Init
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  UpdateData data;
  data.dt = m_isPaused ? 0.0f : h_step;
  data.t = getTime();

  if (m_currentSceneIndex > 0 && m_currentSceneIndex <= m_scenes.size())
  {
    m_renderer->update(m_scenes[m_currentSceneIndex - 1].get(), data);
  }

  // GUI Frame
  {
    ImGui::Begin("Scene Manager");

    // Scene Selector
    {
      std::vector<const char*> names = {"--none--"};
      std::transform(m_scenes.begin(), m_scenes.end(), std::back_inserter(names), [](const std::unique_ptr<Scene>& scene)
                     {
                       return scene->getName();
                     });

      if (ImGui::ListBox("Scene", &m_currentSceneIndex, names.data(), m_scenes.size() + 1, 3))
      {
        selectScene(m_currentSceneIndex);
      }
    }

    // Time Management
    {
      if (ImGui::Button("Restart"))
      {
        selectScene(m_currentSceneIndex);
      }

      ImGui::Checkbox("Paused", &m_isPaused);
    }

    ImGui::End();
  }

  // ImGui Render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainApplication::selectScene(int index)
{
  if (index > m_scenes.size())
  {
    return;
  }

  clearCurrentScene();

  m_currentSceneIndex = index;
  if (index > 0)
  {
    auto& scene = m_scenes[m_currentSceneIndex - 1];
    scene->construct(m_renderer.get());
    m_renderer->start(scene.get());
  }
}

void MainApplication::clearCurrentScene()
{
  m_renderer->getCollisionManager()->clearAll();

  if (m_currentSceneIndex > 0)
  {
    m_scenes[m_currentSceneIndex - 1]->removeChildren();
  }
}
