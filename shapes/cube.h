#ifndef CUBE_H
#define CUBE_H

#include <cstdint>

namespace Shapes {
  class Cube {
    private:
      float cubeVertices[168];
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