#pragma once

#include "Component.hpp"

class Camera final : public Component
{
public:
  Camera();
  ~Camera();

public:
  void processInput(unsigned char press, float dt);
  void mouseMoveCallback(double xpos, double ypos);
  void mouseClickCallback(int button, int action, double xpos, double ypos);
  void scrollBack(float yoffset);

  void reset();

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  glm::mat4 computeView();

private:
  glm::vec3 m_position;
  glm::vec2 m_polar; // Theta - Phi
  float m_distance;

  glm::vec3 m_up;
  glm::vec3 m_forward;
  glm::vec3 m_right;

  bool m_enableRotation;
  double m_lastX;
  double m_lastY;
};

extern Camera* mainCamera;
