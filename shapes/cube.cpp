#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include "cube.h"

// 24 vertices cube to support lighting and to support adding different colors on each side of the cube
Shapes::Cube::Cube() :
  cubeVertices {
    // front face
    -0.5f, 0.5f, 0.5f,
    1.0f, 0.0f, 0.0f, 1.0f, // red
    0.5f, 0.5f, 0.5f,
    1.0f, 0.0f, 0.0f, 1.0f, // red
    -0.5f, -0.5f, 0.5f,
    1.0f, 0.0f, 0.0f, 1.0f, // red
    0.5f, -0.5f, 0.5f,
    1.0f, 0.0f, 0.0f, 1.0f, //red

    // back face
    -0.5f, 0.5f, -0.5f,
    0.0f, 1.0f, 0.0f, 1.0f, // green
    0.5f, 0.5f, -0.5f,
    0.0f, 1.0f, 0.0f, 1.0f, // green
    -0.5f, -0.5f, -0.5f,
    0.0f, 1.0f, 0.0f, 1.0f, // green
    0.5f, -0.5f, -0.5f,
    0.0f, 1.0f, 0.0f, 1.0f, // green

    //left face
    -0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f, 1.0f, // blue
    -0.5f, 0.5f, -0.5f,
    0.0f, 0.0f, 1.0f, 1.0f, // blue
    -0.5f, -0.5f, 0.5f,
    0.0f, 0.0f, 1.0f, 1.0f, // blue
    -0.5f, -0.5f, -0.5f,
    0.0f, 0.0f, 1.0f, 1.0f, // blue

    // right face
    0.5f, 0.5f, 0.5f,
    1.0f, 0.0f, 1.0f, 1.0f, // purple
    0.5f, 0.5f, -0.5f,
    1.0f, 0.0f, 1.0f, 1.0f, // purple
    0.5f, -0.5f, 0.5f,
    1.0f, 0.0f, 1.0f, 1.0f, // purple
    0.5f, -0.5f, -0.5f,
    1.0f, 0.0f, 1.0f, 1.0f, // purple

    // top face
    -0.5f, 0.5f, 0.5f,
    1.0f, 1.0f, 0.0f, 1.0f, // yellow
    0.5f, 0.5f, 0.5f,
    1.0f, 1.0f, 0.0f, 1.0f, // yellow
    -0.5f, 0.5f, -0.5f,
    1.0f, 1.0f, 0.0f, 1.0f, // yellow
    0.5f, 0.5f, -0.5f,
    1.0f, 1.0f, 0.0f, 1.0f, // yellow

    // bottom face
    -0.5f, -0.5f, 0.5f,
    0.32f, 0.5f, 0.32f, 1.0f, // color
    0.5f, -0.5f, 0.5f,
    0.32f, 0.5f, 0.32f, 1.0f, // color
    -0.5f, -0.5f, -0.5f,
    0.32f, 0.5f, 0.32f, 1.0f, // color
    0.5f, -0.5f, -0.5f,
    0.32f, 0.5f, 0.32f, 1.0f, // color
  },
  cubeIndices {
    // front face
    1, 0, 2,
    3, 1, 2,

    // Back Face
    4, 5, 6,
    6, 5, 7,

    // Left Face
    8, 9, 11,
    10, 8, 11,

    // Right Face
    13, 12, 14,
    15, 13, 14,

    // Top Face
    19, 18, 16,
    19, 16, 17,

    // Bot Face
    20, 22, 23,
    20, 23, 21
  }
{
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  GLsizei vertexStride = 7 * sizeof(float);
  // sets to location = 0 in the vertex shader for vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)0);
  // sets to location = 1 in the vertex shader for rgba color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (void*)(3 * sizeof(float)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
}

Shapes::Cube::~Cube()
{
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Cube::Draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
