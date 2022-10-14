#pragma once

#include "Component.hpp"

class Renderable : public Component
{
protected:
  Renderable();

protected:
  virtual void initializeRenderable(std::vector<VertexType> vertices, std::vector<GLuint> index);
  virtual void updateRenderable(Renderer* renderer, glm::mat4 worldToLocal, GLsizei nValues);

protected:
  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;

  // Draw Mode
  GLenum mode;
};
