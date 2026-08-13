#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include "line.h"


Shapes::Line::Line(): 
  lineVertices {
    MathUtils::Vector3 {-0.5f, 0.0f, 0.0f }, 
    MathUtils::Vector3 { 0.5f, 0.0f, 0.0f }
  }, 
  lineIndices { 1, 0 }
{

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

  GLsizei vertexStride = sizeof(lineVertices);
  // sets to location = 0 in the vertex shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices), lineIndices, GL_STATIC_DRAW);
}

Shapes::Line::~Line() {
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Line::Draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, sizeof(lineIndices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}