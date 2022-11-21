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
#include "components/Sphere.hpp"
#include "components/TexturedMesh.hpp"

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

    // // --Bowls--
    // // Ground
    // {
    //   auto ground = std::make_shared<BoxCollider>(
    //     std::make_shared<Box>(glm::vec3(40.0, 10.0, 0.2)));
    //   auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
    //   groundRB->translateBy(glm::vec3(-10.0, 0.0, -3.0));
    //   m_scene->addChild(groundRB);
    // }

    // // Jack
    // {
    //   auto jack = std::make_shared<SphereCollider>(
    //     std::make_shared<Sphere>(0.3f));
    //   auto jackRB = std::make_shared<RigidBody>(jack, 2.0, 0.2, false, true);
    //   jackRB->translateBy(glm::vec3(-22.0, 0.0, -2.0));
    //   m_scene->addChild(jackRB);
    // }

    // // Bowls
    // {
    //   auto bowl1 = std::make_shared<SphereCollider>(
    //     std::make_shared<Sphere>(0.6f));
    //   auto bowl1RB = std::make_shared<RigidBody>(bowl1, 2.0, 0.2, false, true);
    //   bowl1RB->translateBy(glm::vec3(-18.0, -1.0, -2.0));
    //   m_scene->addChild(bowl1RB);
    // }

    // {
    //   auto bowl2 = std::make_shared<SphereCollider>(
    //     std::make_shared<Sphere>(0.6f));
    //   auto bowl2RB = std::make_shared<RigidBody>(bowl2, 2.0, 0.2, false, true);
    //   bowl2RB->translateBy(glm::vec3(0.0, 0.0, 2.0));
    //   bowl2RB->setInitLinearVelocity(glm::vec3(-15.0, 0.0, 2.0));
    //   m_scene->addChild(bowl2RB);
    // }

    // --Pool--
    // Ground
    {
      auto ground = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(20.0, 10.0, 0.2)));
      auto groundRB = std::make_shared<RigidBody>(ground, 1e+6, 0.0, true, false);
      groundRB->translateBy(glm::vec3(0.0, 0.0, -3.0));
      m_scene->addChild(groundRB);
    }

    // Walls
    {
      auto wall1 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0, 7.0, 1.0)));
      auto wall1RB = std::make_shared<RigidBody>(wall1, 1e+6, 0.0, true, false);
      wall1RB->translateBy(glm::vec3(-10.0, 0.0, -2.5));
      m_scene->addChild(wall1RB);
    }    

    {
      auto wall2 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(1.0, 7.0, 1.0)));
      auto wall2RB = std::make_shared<RigidBody>(wall2, 1e+6, 0.0, true, false);
      wall2RB->translateBy(glm::vec3(10.0, 0.0, -2.5));
      m_scene->addChild(wall2RB);
    }

    {
      auto wall3 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(16.5, 1.0, 1.0)));
      auto wall3RB = std::make_shared<RigidBody>(wall3, 1e+6, 0.0, true, false);
      wall3RB->translateBy(glm::vec3(0.0, -5.0, -2.5));
      m_scene->addChild(wall3RB);
    }

    {
      auto wall4 = std::make_shared<BoxCollider>(
        std::make_shared<Box>(glm::vec3(16.5, 1.0, 1.0)));
      auto wall4RB = std::make_shared<RigidBody>(wall4, 1e+6, 0.0, true, false);
      wall4RB->translateBy(glm::vec3(0.0, 5.0, -2.5));
      m_scene->addChild(wall4RB);
    }

    // White Ball
    {
      auto whiteBall = std::make_shared<SphereCollider>(
        std::make_shared<Sphere>(0.5f));
      auto whiteBallRB = std::make_shared<RigidBody>(whiteBall, 5.0, 0.2, false, true);
      whiteBallRB->translateBy(glm::vec3(7.0, 0.0, -2.5));
      whiteBallRB->setInitLinearVelocity(glm::vec3(-18.0, 36.0, 0.0));
      m_scene->addChild(whiteBallRB);
    }

    // Balls
    {
      auto makeBin = [&](glm::vec2 pos)
      {
        auto ball = std::make_shared<SphereCollider>(
          std::make_shared<Sphere>(0.5));
        auto ballRB = std::make_shared<RigidBody>(ball, 5.0, 0.2, false, true);
        ballRB->translateBy(glm::vec3(-3.0 + pos.x, pos.y, -2.5));
        m_scene->addChild(ballRB);
      };

      makeBin(glm::vec2(0.0, 0.0));  // 001
      makeBin(glm::vec2(-1.0, 0.5));  // 002
      makeBin(glm::vec2(-1.0, -0.5)); // 003
      makeBin(glm::vec2(-2.0, 1.0));  // 004
      makeBin(glm::vec2(-2.0, 0.0));  // 005
      makeBin(glm::vec2(-2.0, -1.0)); // 006
    }


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
