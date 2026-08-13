#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include "shape_utils.h"
#include "circle.h"

Shapes::Circle::Circle(uint32_t sectorCount, float radius) :
  sectorCount(sectorCount),
  radius(radius)
{
  circleVertices = createCircleVertices();
  circleIndices = createCircleIndices();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circleVertices.size(), circleVertices.data(), GL_STATIC_DRAW);

  GLsizei vertexStride = 3 * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * circleIndices.size(), circleIndices.data(), GL_STATIC_DRAW);
}

Shapes::Circle::~Circle()
{
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Circle::Draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, circleIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

std::vector<float> Shapes::Circle::createCircleVertices()
{
  std::vector<float> vertices;
  std::vector<float> unitVertices = ShapeUtils::generateUnitCircleCoordinates(sectorCount);

  // Center
  vertices.push_back(0.0f);
  vertices.push_back(0.0f);
  vertices.push_back(0.0f); 

  // 2. Perimeter vertices
  for (size_t i = 0; i < unitVertices.size(); i += 3) {
      float unitX = unitVertices[i];     // X coordinate
      float unitZ = unitVertices[i + 2]; // Z coordinate from unit generator

      vertices.push_back(radius * unitX);
      vertices.push_back(0.0f); 
      vertices.push_back(radius * unitZ);         
  }

  return vertices;
}

std::vector<uint32_t> Shapes::Circle::createCircleIndices()
{
  std::vector<uint32_t> indices;

  uint32_t baseCenterIndex = 0;
  uint32_t i = 1;

  for(uint32_t j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(baseCenterIndex);
      indices.push_back(i);
      indices.push_back(i + 1);
    } else {
      // last triangle
      indices.push_back(i);
      indices.push_back(baseCenterIndex + 1);
      indices.push_back(baseCenterIndex);
    }
  }

  
  return indices;
}
