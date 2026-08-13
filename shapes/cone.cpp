#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include "shape_utils.h"
#include "cone.h"

Shapes::Cone::Cone(uint32_t sectorCount, uint32_t stackCount, float baseRadius, float height): 
  sectorCount(sectorCount),
  stackCount(stackCount),
  baseRadius(baseRadius),
  height(height)
{ 

  coneVertices = createConeVertices();
  coneIndices = createConeIndices();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coneVertices.size(), coneVertices.data(), GL_STATIC_DRAW);
  
  GLsizei vertexPositionStride = 3 * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * coneIndices.size(), coneIndices.data(), GL_STATIC_DRAW);
}

Shapes::Cone::~Cone()
{
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Cone::Draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

std::vector<float> Shapes::Cone::createConeVertices()
{
  std::vector<float> vertices;
  std::vector<float> unitVertices = ShapeUtils::generateUnitCircleCoordinates(sectorCount);

  float baseX = 0.0f, baseY = -height / 2.0f, baseZ = 0.0f;
  vertices.push_back(baseX);
  vertices.push_back(baseY);
  vertices.push_back(baseZ);

  // rings of the cone -- base and sides where base is the 0th ring and the sides are 1st rings to (stackCount - 1)th ring
  for (int i = 0;i < stackCount; ++i) {
    float y = (-height / 2.0f) + ((float)i / stackCount) * height;
    // as height increases, radius decreases to form the cone shape
    float radius = baseRadius * (1.0f - ((float)i / stackCount)); 

    for (int j = 0, k = 0; j < sectorCount; ++j, k += 3) {
      float x = unitVertices[k] * radius;
      float z = unitVertices[k+2] * radius;
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    }
  }

  // tip
  float tipX = 0.0f, tipY = height / 2.0f, tipZ = 0.0f;
  vertices.push_back(tipX);
  vertices.push_back(tipY);
  vertices.push_back(tipZ);

  return vertices;
}

std::vector<uint32_t> Shapes::Cone::createConeIndices()
{
  std::vector<uint32_t> indices;
  
  uint32_t baseCenterIndex = 0;
  // stackCount * sectorCount is the total number of vertices that form all rings around
  // + 1 is the last vertex representing the cone's tip vertex position
  uint32_t tipCenterIndex = (stackCount * sectorCount) + 1;

  // base
  for(uint32_t i = 0; i < sectorCount; ++i) {
    uint32_t v1 = i + 1;
    uint32_t v2 = ((i+1) % sectorCount) + 1;

    indices.push_back(baseCenterIndex);
    indices.push_back(v1);
    indices.push_back(v2);
  }

  // sides
  for (uint32_t i = 0; i < stackCount - 1; ++i) {
    uint32_t beginStackIndex = (i * sectorCount) + 1;
    uint32_t nextStackIndex = ((i+1) * sectorCount) + 1;
    
    for (uint32_t j = 0; j < sectorCount; ++j) {
      uint32_t nextOffset = (j + 1) % sectorCount;

      uint32_t k1 = beginStackIndex + j;
      uint32_t k1Next = beginStackIndex + nextOffset;
      uint32_t k2 = nextStackIndex + j;
      uint32_t k2Next = nextStackIndex + nextOffset;

      indices.push_back(k1);
      indices.push_back(k2);
      indices.push_back(k1Next);

      indices.push_back(k2);
      indices.push_back(k2Next);
      indices.push_back(k1Next);
    }
  }

  // last stack to tip
  uint32_t lastStackIndex = ((stackCount - 1) * sectorCount) + 1;
  for (uint32_t i = 0;i < sectorCount; ++i) {
    uint32_t l1 = lastStackIndex + i;
    uint32_t l2 = lastStackIndex + ((i+1) % sectorCount);
    indices.push_back(tipCenterIndex);
    indices.push_back(l2);
    indices.push_back(l1);
  }

  return indices;
}
