#ifndef CYLINDER_H
#define CYLINDER_H

#include <cstdint>
#include <vector>

#include "debug_vertex.h"

namespace Shapes {
  class Cylinder {
    private:
      std::vector<Shapes::DebugVertex> cylinderVertices;
      std::vector<uint32_t> cylinderIndices;
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;

      uint32_t sectorCount;
      float topRadius;
      float bottomRadius;
      float height;
    public:
      Cylinder(uint32_t sectorCount, float topRadius, float bottomRadius, float height);
      ~Cylinder();
      void Draw();
    private:
      std::vector<Shapes::DebugVertex> createCylinderVertices();
      std::vector<uint32_t> createCylinderIndices();
  };
}

#endif