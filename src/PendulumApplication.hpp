#pragma once

#include "Application.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

#include "components/Component.hpp"
#include "components/Scene.hpp"

class PendulumApplication : public Application {
 public:
  PendulumApplication();
  ~PendulumApplication();

 protected:
  virtual void loop();

 private:
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Scene> m_scene;
};
