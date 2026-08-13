#ifndef DEBUG_VERTEX_H
#define DEBUG_VERTEX_H

#include "math_utils/vector3.h"

namespace Shapes {
  /*
    Each color channel is in a floating point range between 0 to 1
    - 0 means no color or full transparency is applied
    - 1 means color is fully applied and is opaque
  */
  struct DebugColor {
    float r, g, b, a;
    DebugColor() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    DebugColor(float r, float g, float b, float a): r(r), g(g), b(b), a(a) {}
  };

  struct DebugVertex {
    DebugColor color;
    MathUtils::Vector3 position;
    MathUtils::Vector3 normal;
  };
}


#endif