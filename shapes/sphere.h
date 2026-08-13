#ifndef SPHERE_H
#define SPHERE_H

#include <cstdint>
#include <vector>

#include "debug_vertex.h"

namespace Shapes {
  class Sphere {
    private:
      std::vector<Shapes::DebugVertex> sphereVertices;
      std::vector<uint32_t> sphereIndices;
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;

      uint32_t sectorCount;
      uint32_t stackCount;
      float radius;
    public:
      Sphere(uint32_t sectorCount, uint32_t stackCount, float radius);
      ~Sphere();
      void Draw();
    private:
      std::vector<Shapes::DebugVertex> createSphereVertices();
      std::vector<uint32_t> createSphereIndices();
  };
}


#endif