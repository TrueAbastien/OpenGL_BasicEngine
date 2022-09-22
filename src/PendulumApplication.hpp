
#pragma once

#include "Application.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Component.hpp"

class PendulumApplication : public Application {
 public:
  PendulumApplication();

 protected:
  virtual void loop();

 private:
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Scene> m_scene;
};
