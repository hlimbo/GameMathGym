#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include <stddef.h>

#include "quad.h"

Shapes::Quad::Quad(): quadVertices {
  Shapes::DebugVertex {
    Shapes::DebugColor(1.0f, 0.0f, 0.0f, 1.0f),
    MathUtils::Vector3(-0.5f, 0.5f, 0.0f),    // top-left corner
    MathUtils::Vector3(0.0f, 0.0f, -1.0f)
  },
  Shapes::DebugVertex {
    Shapes::DebugColor(1.0f, 0.0f, 0.0f, 1.0f),
    MathUtils::Vector3(-0.5f, -0.5f, 0.0f),   // bottom-left corner
    MathUtils::Vector3(0.0f, 0.0f, -1.0f)
  },
  Shapes::DebugVertex {
    Shapes::DebugColor(1.0f, 0.0f, 0.0f, 1.0f),  
    MathUtils::Vector3(0.5f, 0.5f, 0.0f),     // top right corner
    MathUtils::Vector3(0.0f, 0.0f, -1.0f)
  },
  Shapes::DebugVertex {
    Shapes::DebugColor(1.0f, 0.0f, 0.0f, 1.0f),
    MathUtils::Vector3(0.5f, -0.5f, 0.0f),    // bottom-right corner
    MathUtils::Vector3(0.0f, 0.0f, -1.0f)
  }
}, quadIndices {
  // counter-clockwise order tris
  0, 1, 2,
  1, 3, 2
} {
  glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  GLsizei vertexStride = sizeof(Shapes::DebugVertex);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, color));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, normal));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

}

Shapes::Quad::~Quad() {
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Quad::Draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, sizeof(quadIndices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
