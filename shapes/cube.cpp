#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include <stddef.h>

#include "cube.h"
#include "math_utils/vector3.h"

// 24 vertices cube to support lighting and to support adding different colors on each side of the cube
Shapes::Cube::Cube() :
  cubeVertices {
    // front face
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f }, // red
      MathUtils::Vector3 { -0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, 1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f }, // red
      MathUtils::Vector3 { 0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, 1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f }, // red
      MathUtils::Vector3 { -0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, 1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f }, //red
      MathUtils::Vector3 { 0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, 1.0f }
    },

    // back face
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 1.0f, 0.0f, 1.0f }, // green
      MathUtils::Vector3 { -0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, -1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 1.0f, 0.0f, 1.0f }, // green
      MathUtils::Vector3 { 0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, -1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 1.0f, 0.0f, 1.0f }, // green
      MathUtils::Vector3 { -0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, -1.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 1.0f, 0.0f, 1.0f }, // green
      MathUtils::Vector3 { 0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 0.0f, -1.0f }
    },

    //left face
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 0.0f, 1.0f, 1.0f }, // blue
      MathUtils::Vector3 { -0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { -1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 0.0f, 1.0f, 1.0f }, // blue
      MathUtils::Vector3 { -0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { -1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 0.0f, 1.0f, 1.0f }, // blue
      MathUtils::Vector3 { -0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { -1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.0f, 0.0f, 1.0f, 1.0f }, // blue
      MathUtils::Vector3 { -0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { -1.0f, 0.0f, 0.0f }
    },

    // right face
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 1.0f, 1.0f }, // purple
      MathUtils::Vector3 { 0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { 1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 1.0f, 1.0f }, // purple
      MathUtils::Vector3 { 0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { 1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 1.0f, 1.0f }, // purple
      MathUtils::Vector3 { 0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { 1.0f, 0.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 0.0f, 1.0f, 1.0f }, // purple
      MathUtils::Vector3 { 0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { 1.0f, 0.0f, 0.0f }
    },

    // top face
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
      MathUtils::Vector3 { -0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
      MathUtils::Vector3 { 0.5f, 0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, 1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
      MathUtils::Vector3 { -0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
      MathUtils::Vector3 { 0.5f, 0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, 1.0f, 0.0f }
    },

    // bottom face
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.32f, 0.5f, 0.32f, 1.0f }, // color
      MathUtils::Vector3 { -0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, -1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.32f, 0.5f, 0.32f, 1.0f }, // color
      MathUtils::Vector3 { 0.5f, -0.5f, 0.5f },
      MathUtils::Vector3 { 0.0f, -1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.32f, 0.5f, 0.32f, 1.0f }, // color
      MathUtils::Vector3 { -0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, -1.0f, 0.0f }
    },
    Shapes::DebugVertex {
      Shapes::DebugColor { 0.32f, 0.5f, 0.32f, 1.0f }, // color
      MathUtils::Vector3 { 0.5f, -0.5f, -0.5f },
      MathUtils::Vector3 { 0.0f, -1.0f, 0.0f }
    }
  },
  cubeIndices {
    // front face
    1, 0, 2,
    1, 2, 3,

    // Back Face
    4, 5, 6,
    6, 5, 7,

    // Left Face
    11, 8, 9,
    10, 8, 11,

    // Right Face
    12, 14, 13,
    15, 13, 14,

    // Top Face
    19, 18, 16,
    17, 19, 16,

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

  GLsizei vertexStride = sizeof(Shapes::DebugVertex);
  // sets to location = 0 in the vertex shader for vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, position));
  // sets to location = 1 in the vertex shader for rgba color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, color));
  // sets to location = 2 in the vertex shader for normal
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)offsetof(Shapes::DebugVertex, normal));

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

std::vector<MathUtils::Vector3> Shapes::Cube::GetVertices() const
{
  std::vector<MathUtils::Vector3> vertices;
  for (int i = 0; i < Shapes::Cube::VERTEX_COUNT; ++i) {
    vertices.push_back(cubeVertices[i].position);
  }

  return vertices;
}

std::vector<uint32_t> Shapes::Cube::GetIndices() const
{
  std::vector<uint32_t> indices;

  for (int i = 0; i < Shapes::Cube::INDICES_COUNT; ++i) {
    indices.push_back(cubeIndices[i]);
  }

  return indices;
}
