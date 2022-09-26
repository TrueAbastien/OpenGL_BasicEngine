
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

    m_parentToLocal = glm::rotate((float)previous.first, glm::vec3(-1.0, 0.0, 0.0));
    m_parentToLocal = glm::translate(m_parentToLocal, glm::vec3(0.0, 0.0, -m_scale.z * 0.5));

    data.worldToLocal = data.worldToParent * m_parentToLocal;
    Box::beforeUpdate(renderer, data);
  }
  
private:
 double root_angular_velocity;
 double mass;

 std::pair<double, double> previous;
};

PendulumApplication::PendulumApplication()
    : Application(),
      m_renderer(std::make_unique<Renderer>()),
      m_scene(std::make_unique<Scene>())
{
  glCheckError(__FILE__, __LINE__);

  auto pendulum = std::make_shared<PendulumComponent>(9.81, 1.0, glm::pi<double>() / 5.0, 1.0);
  m_scene->addChild(pendulum);

  auto transform = std::make_shared<EmptyTransform>();
  transform->setLocalModel(glm::vec3(0.0, 0.0, -0.5));
  pendulum->addChild(transform);

  auto dummy = std::make_shared<PendulumComponent>(9.81, 1.0, glm::pi<double>() / 5.0, 1.0);
  transform->addChild(dummy);

  m_renderer->start(m_scene.get());
}

void PendulumApplication::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  float t = getTime();

  // set matrix : projection + view
  m_renderer->setProjection(glm::perspective(
      float(2.0 * atan(getHeight() / 1920.f)), getWindowRatio(), 0.1f, 100.f));
  /*m_renderer->setView(glm::lookAt(glm::vec3(20.0 * sin(t), 20.0 * cos(t), 20.0),
                                  glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));*/
  m_renderer->setView(glm::lookAt(glm::vec3(5.0, 0.0, 0.0),
                                  glm::vec3(0.0, 0.0, 0.0),
                                  glm::vec3(0.0, 0.0, 1.0)));

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  UpdateData data;
  data.dt = (double) getFrameDeltaTime();
  data.t = (double) getTime();
  m_renderer->update(m_scene.get(), data);
}