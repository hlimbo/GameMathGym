#include <gtest/gtest.h>

#include "sat.h"
#include "math_utils/vector3.h"

using namespace CollisionGeometry;
using namespace MathUtils;

TEST(AABB_TESTS, SAT_sanity_test)
{
  std::vector<Vector3> axes = FindAllNormalAxes(std::vector<Vector3>());
  bool hasOverlap = CollisionGeometry::CheckShapesOverlapOneAxisSAT(std::vector<Vector3>(), std::vector<Vector3>(), Vector3());
  bool hasAllOverlaps = CollisionGeometry::CheckShapesOverlapAllAxesSAT(std::vector<Vector3>(), std::vector<Vector3>(), std::vector<Vector3>());
  bool doesOverlap = CollisionGeometry::CheckShapesOverlapSAT(std::vector<Vector3>(), std::vector<Vector3>());

  EXPECT_EQ(0, axes.size());
  EXPECT_FALSE(hasOverlap);
  EXPECT_FALSE(hasAllOverlaps);
  EXPECT_FALSE(doesOverlap);
}