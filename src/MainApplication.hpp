#pragma once

#include "Application.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

#include "components/Component.hpp"
#include "components/Scene.hpp"

class MainApplication final : public Application {
 public:
  MainApplication();
  ~MainApplication();

 protected:
  virtual void loop();

private:
  void selectScene(int index);
  void clearCurrentScene();

 private:
  std::unique_ptr<Renderer> m_renderer;

  std::vector<std::unique_ptr<Scene>> m_scenes;
  int m_currentSceneIndex;

  // Time Manager
  bool m_isPaused;
};
