#ifndef CIRCLE_H
#define CIRCLE_H

#include <cstdint>
#include <vector>

namespace Shapes {
  class Circle {
    private:
      std::vector<float> circleVertices;
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
      std::vector<float> createCircleVertices();
      std::vector<uint32_t> createCircleIndices();
  };
}

#endif