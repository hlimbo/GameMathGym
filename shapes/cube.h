#ifndef CUBE_H
#define CUBE_H

#include <cstdint>
#include <vector>

#include "debug_vertex.h"

namespace MathUtils {
  struct Vector3;
}

namespace Shapes {

  class Cube {
    private:
      static constexpr uint32_t VERTEX_COUNT = 56;
      static constexpr uint32_t INDICES_COUNT = 36;

      Shapes::DebugVertex cubeVertices[VERTEX_COUNT];
      uint32_t cubeIndices[INDICES_COUNT];
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
    public:
      Cube();
      ~Cube();
      void Draw();

      std::vector<MathUtils::Vector3> GetVertices() const;
      std::vector<uint32_t> GetIndices() const;
  };
}

#endif