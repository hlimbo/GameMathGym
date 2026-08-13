#ifndef CIRCLE_H
#define CIRCLE_H

#include <cstdint>
#include <vector>

#include "debug_vertex.h"

namespace Shapes {
  class Circle {
    private:
      std::vector<Shapes::DebugVertex> circleVertices;
      std::vector<uint32_t> circleIndices;
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
      
      uint32_t sectorCount;
      float radius;
    public:
      Circle(uint32_t sectorCount, float radius);
      ~Circle();
      void Draw();
    private:
      std::vector<Shapes::DebugVertex> createCircleVertices();
      std::vector<uint32_t> createCircleIndices();
  };
}

#endif