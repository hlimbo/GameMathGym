#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include "shape_utils.h"
#include "cylinder.h"

Shapes::Cylinder::Cylinder(uint32_t sectorCount, float topRadius, float bottomRadius, float height) :
  sectorCount(sectorCount),
  topRadius(topRadius),
  bottomRadius(bottomRadius),
  height(height)
{
  cylinderVertices = createCylinderVertices();
  cylinderIndices = createCylinderIndices();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);

  GLsizei vertexStride = 3 * sizeof(float);
  // sets to location = 0 in the vertex shader for vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(uint32_t), cylinderIndices.data(), GL_STATIC_DRAW);
}

Shapes::Cylinder::~Cylinder()
{
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Cylinder::Draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

std::vector<float> Shapes::Cylinder::createCylinderVertices()
{
  std::vector<float> vertices;
  std::vector<float> unitVertices = ShapeUtils::generateUnitCircleCoordinates(sectorCount);

  // bottom center base coordinates
  float botX = 0.0f, botY = -height / 2.0f, botZ = 0.0f;
  // top center base coordinates
  float topX = 0.0f, topY = height / 2.0f, topZ = 0.0f;

  vertices.push_back(botX);
  vertices.push_back(botY);
  vertices.push_back(botZ);
  vertices.push_back(topX);
  vertices.push_back(topY);
  vertices.push_back(topZ);

  // bottom cap
  for (int i = 0;i < unitVertices.size(); i += 3) {
    float ux = unitVertices[i];
    float uy = -height / 2.0f;
    float uz = unitVertices[i+2];

    vertices.push_back(bottomRadius * ux);
    vertices.push_back(uy);
    vertices.push_back(bottomRadius * uz);
  }

  // top cap
  for (int i = 0;i < unitVertices.size(); i += 3) {
    float ux = unitVertices[i];
    float uy = height / 2.0f;
    float uz = unitVertices[i+2];

    vertices.push_back(topRadius * ux);
    vertices.push_back(uy);
    vertices.push_back(topRadius * uz);
  }

  // side vertices
  for (int i = 0;i < unitVertices.size(); i += 3) {
    vertices.push_back(bottomRadius * unitVertices[i]);
    vertices.push_back(-height / 2.0f);
    vertices.push_back(bottomRadius * unitVertices[i+2]);
  }

  for (int i = 0;i < unitVertices.size(); i += 3) {
    vertices.push_back(topRadius * unitVertices[i]);
    vertices.push_back(height / 2.0f);
    vertices.push_back(topRadius * unitVertices[i+2]);
  }

  return vertices;
}

std::vector<uint32_t> Shapes::Cylinder::createCylinderIndices()
{
  std::vector<uint32_t> indices;

  uint32_t baseCenterIndex = 0;
  uint32_t topCenterIndex = 1;
  uint32_t i = topCenterIndex + 1;

  // bottom base
  for(uint32_t j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(baseCenterIndex);
      indices.push_back(i);
      indices.push_back(i + 1);
    } else {
      // last triangle
      indices.push_back(i);
      indices.push_back(baseCenterIndex + 2);
      indices.push_back(baseCenterIndex);
    }
  }

  // top base
  uint32_t firstIndexTop = i;
  for (uint32_t j = 0;j < sectorCount; ++j, ++i) {
    if (j < sectorCount - 1) {
      indices.push_back(i+1);
      indices.push_back(i);
      indices.push_back(topCenterIndex);
    } else {
      // last triangle
      indices.push_back(firstIndexTop);
      indices.push_back(i);
      indices.push_back(topCenterIndex);
    }
  }

  // side indices
  uint32_t sideBotStart = i;
  uint32_t sideTopStart = sideBotStart + sectorCount;
  for (uint32_t k = 0;k < sectorCount; ++k) {
    uint32_t b1 = sideBotStart + k;
    uint32_t b2 = sideBotStart + ((k+1) % sectorCount);
    uint32_t t1 = sideTopStart + k;
    uint32_t t2 = sideTopStart + ((k+1) % sectorCount);

    indices.push_back(t1);
    indices.push_back(b2);
    indices.push_back(b1);

    indices.push_back(t1);
    indices.push_back(t2);
    indices.push_back(b2);
  }

  return indices;
}
