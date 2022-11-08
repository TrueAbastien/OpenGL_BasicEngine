#pragma once

#include "Shader.hpp"

#include <optional>
#include <iostream>
#include <map>

// Definitions
constexpr static float h_step = 0.02f;

// Collision Data Response
struct CollisionBodyData
{
  glm::vec3 worldPosition;
  glm::vec3 normal;
};
using CollisionInternalResult = std::pair<CollisionBodyData, CollisionBodyData>;
// - Optional on Collision (inexistant means no Collision)
using CollisionResult = std::optional<CollisionInternalResult>;

// Forward Declaration
class Renderer;
class CollisionManager;
class Physical;
class RigidBody;
class CollisionSolver;

// Forward Inner Using (see. CollisionManager.hpp)
using CurrentTargetCollisions = std::map<Physical*, CollisionInternalResult>;

// Vertex Data Content
struct VertexType
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

// Update Data
struct UpdateData
{
  glm::mat4 parentToWorld;
  glm::mat4 localToWorld;
  float dt;
  float t;
};
