#include "Renderable.hpp"

#include "asset.hpp"

#include "Renderer.hpp"

// ------------------------------------------------------------------------------------------------
Renderable::Renderable()
  : vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
  fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER),
  shaderProgram({vertexShader, fragmentShader}),
  mode(GL_TRIANGLES)
{
  glCheckError(__FILE__, __LINE__);
}

// ------------------------------------------------------------------------------------------------
void Renderable::initializeRenderable(std::vector<VertexType> vertices,
                                      std::vector<GLuint> index)
{
  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << index.size() << std::endl;

  // creation of the vertex array buffer----------------------------------------

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
               index.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // map vbo to shader attributes
  shaderProgram.setAttribute("position", 3, sizeof(VertexType),
                             offsetof(VertexType, position));
  shaderProgram.setAttribute("normal", 3, sizeof(VertexType),
                             offsetof(VertexType, normal));
  shaderProgram.setAttribute("color", 4, sizeof(VertexType),
                             offsetof(VertexType, color));

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);
}

// ------------------------------------------------------------------------------------------------
void Renderable::updateRenderable(Renderer* renderer, glm::mat4 worldToLocal, GLsizei nValues)
{
  shaderProgram.use();

  // send uniforms
  shaderProgram.setUniform("projection", renderer->getProjection());
  shaderProgram.setUniform("view", renderer->getView() * worldToLocal);

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glCheckError(__FILE__, __LINE__);
  glDrawElements(mode,             // mode
                 nValues,          // count
                 GL_UNSIGNED_INT,  // type
                 NULL              // element array buffer offset
  );

  glBindVertexArray(0);

  shaderProgram.unuse();
}
