#include "Camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "asset.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Renderer.hpp"

Camera* mainCamera = nullptr;

// ------------------------------------------------------------------------------------------------
namespace CameraDefinitions
{
  const glm::vec2 linearVelocity = glm::vec2(10.0, 10.0);
  const glm::vec2 angularVelocity = glm::vec2(glm::pi<float>() / 1'000.0, glm::pi<float>() / 1'000.0);
  const float scrollVelocity = 0.5f;
}

// ------------------------------------------------------------------------------------------------
void inputCallback(GLFWwindow* window, float dt)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (mainCamera) mainCamera->processInput(
    ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ? (1 << 0) : 0) |
    ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? (1 << 1) : 0) |
    ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ? (1 << 2) : 0) |
    ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ? (1 << 3) : 0) |
    ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) ? (1 << 4) : 0),
    dt);
}

// ------------------------------------------------------------------------------------------------
Camera::Camera()
{
  mainCamera = this;

  reset();
}

// ------------------------------------------------------------------------------------------------
Camera::~Camera()
{
  if (mainCamera == this)
  {
    mainCamera = nullptr;
  }
}

// ------------------------------------------------------------------------------------------------
void Camera::processInput(unsigned char press, float dt)
{
  using namespace CameraDefinitions;

  if ((press & (1 << 0)) != 0) m_position += m_up * linearVelocity.y * dt;
  if ((press & (1 << 1)) != 0) m_position -= m_up * linearVelocity.y * dt;
  if ((press & (1 << 2)) != 0) m_position += m_right * linearVelocity.x * dt;
  if ((press & (1 << 3)) != 0) m_position -= m_right * linearVelocity.x * dt;

  if ((press & (1 << 4)) != 0) reset();
}

// ------------------------------------------------------------------------------------------------
void Camera::mouseMoveCallback(double xpos, double ypos)
{
  if (!m_enableRotation) return;

  using namespace CameraDefinitions;

  m_polar += glm::vec2(
    angularVelocity.x * (float)(xpos - m_lastX),
    angularVelocity.y * (float)(m_lastY - ypos));

  m_lastX = xpos;
  m_lastY = ypos;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  constexpr float maxInclinaison = glm::pi<float>() - 1e-2f;
  if (m_polar.y > maxInclinaison)
    m_polar.y = maxInclinaison;
  else if (m_polar.y < 1e-2f)
    m_polar.y = 0;
}

// ------------------------------------------------------------------------------------------------
void Camera::mouseClickCallback(int button, int action, double xpos, double ypos)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if (action == GLFW_PRESS)
    {
      m_enableRotation = true;

      m_lastX = xpos;
      m_lastY = ypos;
    }
    else if (action == GLFW_RELEASE)
    {
      m_enableRotation = false;
    }
  }
}

// ------------------------------------------------------------------------------------------------
void Camera::scrollBack(float yoffset)
{
  m_distance -= yoffset;

  if (m_distance < 0.1f)
    m_distance = 0.1f;
  else if (m_distance > 100.0f)
    m_distance = 100.0f;
}

// ------------------------------------------------------------------------------------------------
void Camera::reset()
{
  m_position = glm::vec3(0.0);
  m_polar = glm::vec2(0.0, glm::pi<float>() * 0.5f);
  m_distance = 10.0f;

  m_up = glm::vec3(0.0, 0.0, 1.0);
  m_forward = glm::vec3(1.0, 0.0, 0.0);
  m_right = glm::vec3(0.0, 1.0, 0.0);

  m_enableRotation = false;
  m_lastX = 0.0;
  m_lastY = 0.0;
}

// ------------------------------------------------------------------------------------------------
void Camera::beforeInitialize(Renderer* renderer)
{
  // TODO: compute distance ?
}

// ------------------------------------------------------------------------------------------------
void Camera::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  // Process Inputs
  inputCallback(renderer->getWindow(), data.dt);

  // Update Transforms
  glm::mat4 mat = computeView();
  setLocalToParent(glm::inverse(mat));
  renderer->setView(mat);
}

// ------------------------------------------------------------------------------------------------
glm::mat4 Camera::computeView()
{
  m_forward = glm::normalize(glm::vec3(
    glm::sin(m_polar.y) * glm::cos(m_polar.x),
    glm::sin(m_polar.y) * glm::sin(m_polar.x),
    glm::cos(m_polar.y)
  ));
  m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0, 0, 1)));
  m_up = glm::normalize(glm::cross(m_right, m_forward));

  return glm::lookAt(m_position + m_forward * m_distance, m_position, m_up);
}
