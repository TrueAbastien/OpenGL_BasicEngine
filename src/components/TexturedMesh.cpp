#include "TexturedMesh.hpp"

#include "asset.hpp"

#include "Renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vendors/stb_image.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// ------------------------------------------------------------------------------------------------
TexturedMesh::TexturedMesh(const std::string& objFile, const std::string& texFile,
                           float scale, glm::vec3 offset, glm::vec3 rot)
  : vertexShader(SHADER_DIR "/texture.vert", GL_VERTEX_SHADER),
  fragmentShader(SHADER_DIR "/texture.frag", GL_FRAGMENT_SHADER),
  shaderProgram({vertexShader, fragmentShader}),
  m_texfilePath(RESSOURCES_DIR + texFile)
{

  // Read OBJ File
  std::ifstream obj(RESSOURCES_DIR + objFile);
  if (!obj.is_open()) return;

  using SymbolVn = glm::vec3;
  using SymbolV = glm::vec3;
  using SymbolVt = glm::vec2;
  using SubSymbolF = std::array<int, 3>;
  using SymbolF = std::array<SubSymbolF, 3>;

  std::vector<SymbolVn> normals;
  std::vector<SymbolV> vertices;
  std::vector<SymbolVt> textures;
  std::vector<SymbolF> faces;

  glm::mat4 tr =
    glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0)) *
    glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0)) *
    glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
  tr = glm::translate(glm::mat4(1.0), offset) * glm::scale(tr, glm::vec3(scale));

  for (std::string line; std::getline(obj, line); )
  {
    // Faces
    if (line.find("f ") == 0)
    {
      if (line.find("//") != std::string::npos) return;
      std::replace(line.begin(), line.end(), '/', ' ');

      std::stringstream ss(line.c_str() + 2);
      std::vector<int> indexes(0);

      while (!ss.eof())
      {
        int i; ss >> i;
        indexes.push_back(i);
      }
      
      if (indexes.size() % 3 != 0) return;

      std::vector<SubSymbolF> vtxIndexes(0);
      for (int ii = 0; ii < indexes.size(); ii += 3)
      {
        vtxIndexes.push_back(SubSymbolF{indexes[ii], indexes[ii + 1], indexes[ii + 2]});
      }

      if (vtxIndexes.size() < 3) return;

      for (int jj = 0; jj < vtxIndexes.size() - 2; ++jj)
      {
        faces.push_back(SymbolF{vtxIndexes[0], vtxIndexes[jj + 1], vtxIndexes[jj + 2]});
      }
    }

    // Vertices
    else if (line.find("v ") == 0)
    {
      std::stringstream ss(line.c_str() + 2);
      std::vector<float> values(0);

      while (!ss.eof())
      {
        float v; ss >> v;
        values.push_back(v);
      }

      if (values.size() != 3) return;

      vertices.push_back(tr * glm::vec4(values[0], values[1], values[2], 1.0f));
    }

    // Normals
    else if (line.find("vn ") == 0)
    {
      std::stringstream ss(line.c_str() + 3);
      std::vector<float> values(0);

      while (!ss.eof())
      {
        float v; ss >> v;
        values.push_back(v);
      }

      if (values.size() != 3) return;

      normals.push_back(glm::vec3(values[0], values[1], values[2]));
    }

    // Textures
    else if (line.find("vt ") == 0)
    {
      std::stringstream ss(line.c_str() + 3);
      std::vector<float> values(0);

      while (!ss.eof())
      {
        float v; ss >> v;
        values.push_back(v);
      }

      if (values.size() != 2) return;

      textures.push_back(glm::vec2(values[0], values[1]));
    }
  }

  // Close OBJ
  obj.close();

  // Construct Mesh
  GLuint travelingIndex = 0;
  for (const SymbolF& face : faces)
  {
    for (const SubSymbolF& faceIdx : face)
    {
      // Vertex
      int vertexIndex = faceIdx.at(0);
      if (vertexIndex > vertices.size()) return;
      glm::vec3 vertex = vertices[vertexIndex - 1];

      // Texture
      int textureIndex = faceIdx.at(1);
      if (textureIndex > textures.size()) return;
      glm::vec2 texture = textures[textureIndex - 1];

      // Normal
      int normalIndex = faceIdx.at(2);
      if (normalIndex > normals.size()) return;
      glm::vec3 normal = normals[normalIndex - 1];

      // Faces
      m_vertices.push_back(VertexTextured{vertex, normal, texture});
      m_indexes.push_back(travelingIndex);
      travelingIndex++;
    }
  }

  m_nFaces = faces.size();
}

// ------------------------------------------------------------------------------------------------
void TexturedMesh::beforeInitialize(Renderer* renderer)
{
  // Read Tex File
  {
    std::ifstream tex(m_texfilePath);
    if (!tex.is_open()) return;
    tex.close();
  }

  // Generate Texture
  {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(m_texfilePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
      std::cout << "Failed to load texture: " << m_texfilePath.c_str() << std::endl;
      return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  }

  // creation of the vertex array buffer----------------------------------------

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexTextured),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexes.size() * sizeof(GLuint),
               m_indexes.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // map vbo to shader attributes
  shaderProgram.setAttribute("position", 3, sizeof(VertexTextured),
                             offsetof(VertexTextured, position));
  shaderProgram.setAttribute("normal", 3, sizeof(VertexTextured),
                             offsetof(VertexTextured, normal));
  shaderProgram.setAttribute("uv", 2, sizeof(VertexTextured),
                             offsetof(VertexTextured, uv));

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);
}

// ------------------------------------------------------------------------------------------------
void TexturedMesh::beforeUpdate(Renderer* renderer, UpdateData& data)
{
  shaderProgram.use();

  shaderProgram.setUniform("tex", 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  // send uniforms
  shaderProgram.setUniform("projection", renderer->getProjection());
  shaderProgram.setUniform("view", renderer->getView() * data.localToWorld);

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_TRIANGLES,     // mode
                 m_nFaces * 3,     // count
                 GL_UNSIGNED_INT,  // type
                 NULL              // element array buffer offset
  );

  glBindVertexArray(0);
  
  // TODO: unbind texture ?

  shaderProgram.unuse();
}
