#pragma once

#include "Component.hpp"

class TexturedMesh final : public Component
{
public:
  struct VertexTextured
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

public:
  TexturedMesh(const std::string& objFile, const std::string& texFile,
               float scale = 1.0f, glm::vec3 offset = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

  std::vector<VertexTextured> getVertices() const;

protected:
  void beforeInitialize(Renderer* renderer) override;
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  // Shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;

  // Texturing
  size_t m_nFaces;
  std::string m_texfilePath;
  unsigned int m_texture;

  // Infos
  std::vector<VertexTextured> m_vertices;
  std::vector<GLuint> m_indexes;
};