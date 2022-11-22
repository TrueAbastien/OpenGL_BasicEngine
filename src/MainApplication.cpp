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
  // TO EXPAND

  // RigidBody
  /*
  {
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0, 1.0, 1.0)));
      auto wrapper = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true);
      wrapper->translateBy(glm::vec3(0.0, 0.0, -3.0));
      wrapper->rotateBy(glm::vec3(0.2f, 0.0, 0.0));
      m_scene->addChild(wrapper);
    }

    {
      auto sphereGround = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(1.0f));
      auto sphereWrapper = std::make_shared<RigidBody>(sphereGround, 1e+6, 0.99, true);
      sphereWrapper->translateBy(glm::vec3(0.0, -2.0, -3.0));

      m_scene->addChild(sphereWrapper);
    }

    {
      auto box = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0f, 1.0f, 1.0f)));
      auto rigidBody = std::make_shared<RigidBody>(box, 10.0, 0.8);
      rigidBody->translateBy(glm::vec3(0.0, 0.0, 5.0));
      //rigidBody->rotateBy(glm::vec3(0.0, 0.0, glm::pi<float>()));
      rigidBody->addForce(RigidBody::ExternalForce // Gravity
                          {
                            glm::vec3(0.0, 0.0, 0.0), // Position
                            glm::vec3(0.0, 0.0, -9.81) // Force
                            * rigidBody->getMass()
                          });
      m_scene->addChild(rigidBody);
    }

    {
      auto box = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(2.0f, 2.0f, 0.5f)));
      auto rigidBody = std::make_shared<RigidBody>(box, 10.0, 1.0);
      rigidBody->translateBy(glm::vec3(0.0, 0.0, 15.0));
      //rigidBody->rotateBy(glm::vec3(0.0, 0.0, glm::pi<float>()));
      rigidBody->addForce(RigidBody::ExternalForce // Gravity
                          {
                            glm::vec3(0.0, 0.0, 0.0), // Position
                            glm::vec3(0.0, 0.0, -9.81) // Force
                            * rigidBody->getMass()
                          });
      m_scene->addChild(rigidBody);
    }
    
    {
      auto sphere = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(1.0f));
      auto sphereRigidBody = std::make_shared<RigidBody>(sphere, 10.0, 0.99);
      sphereRigidBody->translateBy(glm::vec3(0.0, -2.0, 1.0));
      sphereRigidBody->addForce(RigidBody::ExternalForce // Gravity
                          {
                            glm::vec3(0.0, 0.0, 0.0), // Position
                            glm::vec3(0.0, 0.0, -9.81) // Force
                            * sphereRigidBody->getMass()
                          });
      m_scene->addChild(sphereRigidBody);
    }
  }
  */

  //{
  //  {
  //    auto ground = std::make_shared<BoxCollider>(
  //      std::make_shared<Box>(glm::vec3(10.0, 10.0, 1.0)));
  //    auto wrapper = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true);
  //    wrapper->translateBy(glm::vec3(0.0, 0.0, -3.0));
  //    //wrapper->rotateBy(glm::vec3(0.1f, 0.0, 0.0));
  //    m_scene->addChild(wrapper);
  //  }

  //  /*{
  //    auto sphere = std::make_shared<SphereCollider>(
  //      std::make_shared<Sphere>(1.0f));
  //    auto sphereRB = std::make_shared<RigidBody>(sphere, 10.0, 0.5, false, true);
  //    sphereRB->translateBy(glm::vec3(0.0, 0.0, 5.0));
  //    m_scene->addChild(sphereRB);
  //  }*/

  //  {
  //    auto box = std::make_shared<BoxCollider>(
  //      std::make_shared<Box>(glm::vec3(1.0f, 1.0f, 2.0f)));
  //    auto boxRB = std::make_shared<RigidBody>(box, 10.0, 0.5, false, true);
  //    boxRB->translateBy(glm::vec3(0.0, 2.0, 5.0));
  //    m_scene->addChild(boxRB);
  //  }
  //}

  {
    //// Ground
    //{
    //  auto ground = std::make_shared<BoxCollider>(
    //    std::make_shared<Box>(glm::vec3(3.0, 20.0, 0.5)));
    //  auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
    //  groundRB->translateBy(glm::vec3(0.0, 0.0, -3.0));
    //  m_scene->addChild(groundRB);
    //}

    //// Ball
    //{
    //  auto ball = std::make_shared<SphereCollider>(
    //    std::make_shared<Sphere>(1.0));
    //  auto ballRB = std::make_shared<RigidBody>(ball, 10.0, 0.2, false, true);
    //  ballRB->translateBy(glm::vec3(0.0, -10.0, -2.0));
    //  ballRB->addForce(RigidBody::ExternalForce
    //                   {
    //                     glm::vec3(0.0, 0.0, 0.3), // Position
    //                     glm::vec3(0.0, 10.0, 0.0) // Force
    //                   });
    //  m_scene->addChild(ballRB);
    //}

    //// Bowling Pin
    //{
    //  auto makePin = [&](glm::vec2 pos)
    //  {
    //    auto pin = std::make_shared<BoxCollider>(
    //      std::make_shared<Box>(glm::vec3(0.5, 0.5, 3)));
    //    auto pinRB = std::make_shared<RigidBody>(pin, 5.0, 0.2, false, true);
    //    pinRB->translateBy(glm::vec3(pos.x, 5.0 + pos.y, 0.0));
    //    m_scene->addChild(pinRB);
    //  };

    //  makePin(glm::vec2(0.0, 0.0));  // 001
    //  makePin(glm::vec2(0.5, 1.0));  // 002
    //  makePin(glm::vec2(-0.5, 1.0)); // 003
    //  makePin(glm::vec2(1.0, 2.0));  // 004
    //  makePin(glm::vec2(0.0, 2.0));  // 005
    //  makePin(glm::vec2(-1.0, 2.0)); // 006
    //}


    // // Ground
    // {
    //   auto ground = std::make_shared<BoxCollider>(
    //     std::make_shared<Box>(glm::vec3(3.0, 20.0, 0.5)));
    //   auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
    //   groundRB->translateBy(glm::vec3(0.0, 0.0, -3.0));
    //   m_scene->addChild(groundRB);
    // }

    // // Ball
    // {
    //   auto ball = std::make_shared<SphereCollider>(
    //     std::make_shared<Sphere>(1.0));
    //   auto ballRB = std::make_shared<RigidBody>(ball, 10.0, 0.2, false, true);
    //   ballRB->translateBy(glm::vec3(0.0, -10.0, -2.0));
    //   ballRB->addForce(RigidBody::ExternalForce
    //                    {
    //                      glm::vec3(0.0, 0.0, 0.3), // Position
    //                      glm::vec3(0.0, 10.0, 0.0) // Force
    //                    });
    //   m_scene->addChild(ballRB);
    // }

    // // Bowling Pin
    // {
    //   auto makePin = [&](glm::vec2 pos)
    //   {
    //     auto pin = std::make_shared<BoxCollider>(
    //       std::make_shared<Box>(glm::vec3(0.5, 0.5, 3)));
    //     auto pinRB = std::make_shared<RigidBody>(pin, 5.0, 0.2, false, true);
    //     pinRB->translateBy(glm::vec3(pos.x, 5.0 + pos.y, 0.0));
    //     m_scene->addChild(pinRB);
    //   };

    //   makePin(glm::vec2(0.0, 0.0));  // 001
    //   makePin(glm::vec2(0.5, 1.0));  // 002
    //   makePin(glm::vec2(-0.5, 1.0)); // 003
    //   makePin(glm::vec2(1.0, 2.0));  // 004
    //   makePin(glm::vec2(0.0, 2.0));  // 005
    //   makePin(glm::vec2(-1.0, 2.0)); // 006
    // }

    // TEST //
    /*{
      auto mesh = std::make_shared<TexturedMesh>("/mesh/pin.obj", "/texture/bowling.jpg",
                                                 0.1f,
                                                 glm::vec3(0.0, 1.7, 0.0),
                                                 glm::vec3(0.5, 0.0, 0.0) * glm::pi<float>());
      m_scene->addChild(mesh);
    }

    {
      auto mesh = std::make_shared<TexturedMesh>("/mesh/ball.obj", "/texture/bowling.jpg",
                                                 0.1f,
                                                 glm::vec3(0.0, -1.0, 1.0));
      m_scene->addChild(mesh);
    }*/
  }

  //m_renderer->start(m_scene.get());
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
