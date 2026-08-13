#ifndef QUAD_H
#define QUAD_H

#include <cstdint>

namespace Shapes {
  class Quad {
    private:
      float quadVertices[12];
      uint32_t quadIndices[6];
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
    public:
      Quad();
      ~Quad();
      void Draw();
  };
}

#endif