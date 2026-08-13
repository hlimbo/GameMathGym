#ifndef SPHERE_H
#define SPHERE_H

#include <cstdint>
#include <vector>

namespace Shapes {
  class Sphere {
    private:
      std::vector<float> sphereVertices;
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
      std::vector<float> createSphereVertices();
      std::vector<uint32_t> createSphereIndices();
  };
}


#endif