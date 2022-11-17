#include "PendulumApplication.hpp"

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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class PendulumComponent final : public Box
{
public:
  PendulumComponent(double _gravity, double _length, double _theta, double _mass)
     : Box(glm::vec3(0.1, 0.1, _length)),
       root_angular_velocity(_gravity / _length),
       mass(_mass),
       previous({_theta, 1.0})
  {
  }

protected:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override
  {
    previous.second -= root_angular_velocity * glm::sin(previous.first) * data.dt;
    previous.first += previous.second * data.dt;

    m_localToParent = glm::rotate((float)previous.first, glm::vec3(-1.0, 0.0, 0.0));
    m_localToParent = glm::translate(m_localToParent, glm::vec3(0.0, 0.0, -m_scale.z * 0.5));

    data.localToWorld = data.parentToWorld * m_localToParent;
    Box::beforeUpdate(renderer, data);
  }
  
private:
 double root_angular_velocity;
 double mass;

 std::pair<double, double> previous;
};

PendulumApplication::PendulumApplication()
    : Application()
{
  m_renderer = std::make_unique<Renderer>(window);
  m_scene = std::make_unique<Scene>(m_renderer.get());

  glCheckError(__FILE__, __LINE__);

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();

  // Pendulum
  /*{
    auto pendulum = std::make_shared<PendulumComponent>(9.81, 1.0, glm::pi<double>() / 5.0, 1.0);
    m_scene->addChild(pendulum);

    auto transform = std::make_shared<EmptyTransform>();
    transform->setLocalModel(glm::vec3(0.0, 0.0, -0.5));
    pendulum->addChild(transform);

    auto dummy = std::make_shared<PendulumComponent>(9.81, 1.0, glm::pi<double>() / 5.0, 1.0);
    transform->addChild(dummy);
  }*/

  // RigidBody
  {
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(10.0, 10.0, 1.0)));
      auto wrapper = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true);
      wrapper->translateBy(glm::vec3(0.0, 0.0, -3.0));
      wrapper->rotateBy(glm::vec3(0.2f, 0.0, 0.0));
      m_scene->addChild(wrapper);
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
  }

  // Spring
  /*{
    float force = 10.0f;

    auto cube1 = std::make_shared<BoxCollider>(
      std::make_shared<Box>(glm::vec3(1.0)));
    auto rb1 = std::make_shared<RigidBody>(cube1, 10.0, 1.0);
    rb1->translateBy(glm::vec3(0.0, -10.0, 0.0));
    rb1->addForce(RigidBody::ExternalForce
    (
      glm::vec3(0.0), glm::vec3(0.0, force, 0.0)
    ));
    m_scene->addChild(rb1);

    auto cube2 = std::make_shared<BoxCollider>(
      std::make_shared<Box>(glm::vec3(1.0)));
    auto rb2 = std::make_shared<RigidBody>(cube2, 10.0, 1.0);
    rb2->translateBy(glm::vec3(0.0, 10.0, 0.0));
    rb2->addForce(RigidBody::ExternalForce
    (
      glm::vec3(0.0), glm::vec3(0.0, -force, 0.0)
    ));
    m_scene->addChild(rb2);
  }*/

  m_renderer->start(m_scene.get());
}

PendulumApplication::~PendulumApplication()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void PendulumApplication::loop() {
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
  //data.dt = getFrameDeltaTime();
  data.dt = h_step;
  data.t = getTime();
  m_renderer->update(m_scene.get(), data);

  {
    // GUI Frame
    //ImGui::ShowDemoWindow();
  }

  // ImGui Render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
