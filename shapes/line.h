#ifndef LINE_H
#define LINE_H

// Using this for uint32_t over GLuint as there is less ovehead than
// including the glad header file. Also, the uint32_t is guaranteed to be
// an unsigned int of 32 bits like GLuint
// https://en.cppreference.com/cpp/header/cstdint
// https://wikis.khronos.org/opengl/OpenGL_Type
#include <cstdint>
#include "math_utils/vector3.h"

namespace Shapes {
  class Line {
    private:
      MathUtils::Vector3 lineVertices[2];
      uint32_t lineIndices[2];
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
    public:
      Line();
      ~Line();
      void Draw();

  };
}


#endif