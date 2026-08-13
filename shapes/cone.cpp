#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include <stddef.h>

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::DebugVertex) * coneVertices.size(), coneVertices.data(), GL_STATIC_DRAW);
  
  GLsizei vertexPositionStride = sizeof(Shapes::DebugVertex);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, color));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, normal));

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

std::vector<Shapes::DebugVertex> Shapes::Cone::createConeVertices()
{
  std::vector<Shapes::DebugVertex> vertices;
  std::vector<float> unitVertices = ShapeUtils::generateUnitCircleCoordinates(sectorCount);

  Shapes::DebugVertex baseCenter {
    Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f },
    MathUtils::Vector3 { 0.0f, -height / 2.0f, 0.0f },
    MathUtils::Vector3 { 0.0f, -1.0f, 0.0f }
  };
  vertices.push_back(baseCenter);

  // rings of the cone -- base and sides where base is the 0th ring and the sides are 1st rings to (stackCount - 1)th ring
  for (int i = 0;i < stackCount; ++i) {
    float y = (-height / 2.0f) + ((float)i / stackCount) * height;
    // as height increases, radius decreases to form the cone shape
    float radius = baseRadius * (1.0f - ((float)i / stackCount)); 

    for (int j = 0, k = 0; j < sectorCount; ++j, k += 3) {
      float x = unitVertices[k] * radius;
      float z = unitVertices[k+2] * radius;

      MathUtils::Vector3 normal;
      // At Base ring
      if (i == 0) {
        normal = MathUtils::Vector3(0.0f, -1.0f, 0.0f);
      } else {
        // Assumption: cylinder is formed on the origin
        // where its tip is pointing upwards in global y direction (0, 1, 0). If it's not pointing upwards in global y, I would need to do a vector projection to find out the closest point to the current position that is being calculated.
        normal = MathUtils::Vector3(unitVertices[k], 0.0f, unitVertices[k+2]);
      }

      Shapes::DebugVertex vertex {
        Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 1.0f },
        MathUtils::Vector3 { x, y, z },
        normal,
      };
      vertices.push_back(vertex);
    }
  }

  Shapes::DebugVertex tip {
    Shapes::DebugColor { 1.0f, 0.0f, 0.0f, 0.0f },
    MathUtils::Vector3 { 0.0f, height / 2.0f, 0.0f },
    MathUtils::Vector3 { 0.0f, 1.0f, 0.0f }
  };
  vertices.push_back(tip);

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
