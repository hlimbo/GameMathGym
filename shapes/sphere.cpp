#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h> // web builds
#else
  #include <glad/glad.h> // desktop builds
#endif

#include <stddef.h>

#include "math_utils/matrix4.h"
#include "sphere.h"

Shapes::Sphere::Sphere(uint32_t sectorCount, uint32_t stackCount, float radius) :
  sectorCount(sectorCount),
  stackCount(stackCount),
  radius(radius)
{
  sphereVertices = createSphereVertices();
  sphereIndices = createSphereIndices();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::DebugVertex) * sphereVertices.size(), sphereVertices.data(), GL_STATIC_DRAW);
  
  GLsizei vertexPositionStride = sizeof(Shapes::DebugVertex);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, color));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexPositionStride, (void*)offsetof(Shapes::DebugVertex, normal));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * sphereIndices.size(), sphereIndices.data(), GL_STATIC_DRAW);
}

Shapes::Sphere::~Sphere()
{
  glBindVertexArray(0); // unbind VAO from global opengl context
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO from global opengl context
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO from global opengl context

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Shapes::Sphere::Draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

std::vector<Shapes::DebugVertex> Shapes::Sphere::createSphereVertices()
{
  std::vector<Shapes::DebugVertex> vertices;

  // stackStep <= stackCount accounts for top pole
  for (int stackStep = 0; stackStep <= stackCount; ++stackStep) {
    // phi range is from -PI / 2 to PI / 2
    float phi = MathUtils::PI * ((float)stackStep / stackCount) - (MathUtils::PI / 2.0f);
    float sinPhi = (float)std::sin(phi);
    float cosPhi = (float)std::cos(phi);

    for (int sectorStep = 0; sectorStep < sectorCount; ++sectorStep) {
      // theta range is from 0 to 2 * PI
      float theta = 2.0f * MathUtils::PI * ((float)sectorStep / sectorCount);
      float sinTheta = (float)std::sin(theta);
      float cosTheta = (float)std::cos(theta);

      float nx = cosPhi * cosTheta;
      float ny = sinPhi;
      float nz = cosPhi * sinTheta;
      float x = radius * nx;
      float y = radius * ny;
      float z = radius * nz;

      Shapes::DebugVertex vertex {
        Shapes::DebugColor(0.85f, 0.25f, 0.35f, 1.0f),
        MathUtils::Vector3(x, y, z),
        MathUtils::Vector3(nx, ny, nz)
      };

      vertices.push_back(vertex);
    } 
  }

  return vertices;
}

std::vector<uint32_t> Shapes::Sphere::createSphereIndices()
{
  std::vector<uint32_t> indices;

  for (uint32_t stackStep = 1; stackStep <= stackCount; ++stackStep) {
    for (uint32_t sectorStep = 0; sectorStep < sectorCount; ++sectorStep) {
      uint32_t nextSector = (sectorStep + 1) % sectorCount;

      // bottom
      uint32_t v1 = ((stackStep-1) * sectorCount) + sectorStep;
      uint32_t v1Next = ((stackStep-1) * sectorCount) + nextSector;

      // top
      uint32_t v2 = (stackStep * sectorCount) + sectorStep;
      uint32_t v2Next = (stackStep * sectorCount) + nextSector;

      indices.push_back(v1);
      indices.push_back(v2);
      indices.push_back(v2Next);

      indices.push_back(v1Next);
      indices.push_back(v1);
      indices.push_back(v2Next);
    }
  }

  return indices;
}
