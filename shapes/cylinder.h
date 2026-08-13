#ifndef CYLINDER_H
#define CYLINDER_H

#include <cstdint>
#include <vector>

namespace Shapes {
  class Cylinder {
    private:
      std::vector<float> cylinderVertices;
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
      std::vector<float> createCylinderVertices();
      std::vector<uint32_t> createCylinderIndices();
  };
}

#endif