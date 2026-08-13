#ifndef CONE_H
#define CONE_H

#include <cstdint>
#include <vector>

namespace Shapes {
  class Cone {
    private:
      std::vector<float> coneVertices;
      std::vector<uint32_t> coneIndices;
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;

      uint32_t sectorCount;
      uint32_t stackCount;
      float baseRadius;
      float height;
    public:
      Cone(uint32_t sectorCount, uint32_t stackCount, float baseRadius, float height);
      ~Cone();
      void Draw();
    private:
      std::vector<float> createConeVertices();
      std::vector<uint32_t> createConeIndices();
  };
}

#endif