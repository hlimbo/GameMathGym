#ifndef CUBE_H
#define CUBE_H

#include <cstdint>

#include "debug_vertex.h"

namespace Shapes {
  class Cube {
    private:
      Shapes::DebugVertex cubeVertices[56];
      uint32_t cubeIndices[36];
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
    public:
      Cube();
      ~Cube();
      void Draw();
  };
}

#endif