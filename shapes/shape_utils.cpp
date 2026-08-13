#include <assert.h>
#include <cmath>

// TODO: either remove PI constexpr from this header file and move to a constants file or use PI from a standard c++ library...
#include "math_utils/matrix4.h"
#include "shape_utils.h"

std::vector<float> ShapeUtils::generateUnitCircleCoordinates(int sectorCount) {
  assert(sectorCount > 0);

  std::vector<float> vertices;

  float sectorStep = (2.0f * MathUtils::PI) / sectorCount;
  for (int i = 0;i < sectorCount; ++i) {
    float sectorAngle = i * sectorStep;
    float x = (float)std::cos(sectorAngle);
    float y = 0.0f;
    float z = (float)std::sin(sectorAngle);

    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);

  }

  return vertices;
}