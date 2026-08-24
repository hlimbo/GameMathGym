#include <gtest/gtest.h>

#include "aabb.h"
#include "math_utils/vector3.h"

using namespace CollisionGeometry;
using namespace MathUtils;

TEST(AABB_TESTS, Constructors)
{
  AABB aabbDefault;

  EXPECT_FLOAT_EQ(0.0f, aabbDefault.minX);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.minY);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.minZ);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.maxX);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.maxY);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.maxZ);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.width);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.height);
  EXPECT_FLOAT_EQ(0.0f, aabbDefault.depth);

  AABB aabb(1.0f, 2.0f, 3.0f, 3.0f, 4.0f, 5.0f);

  EXPECT_FLOAT_EQ(1.0f, aabb.minX);
  EXPECT_FLOAT_EQ(2.0f, aabb.minY);
  EXPECT_FLOAT_EQ(3.0f, aabb.minZ);
  EXPECT_FLOAT_EQ(3.0f, aabb.maxX);
  EXPECT_FLOAT_EQ(4.0f, aabb.maxY);
  EXPECT_FLOAT_EQ(5.0f, aabb.maxZ);
  EXPECT_FLOAT_EQ(2.0f, aabb.width);
  EXPECT_FLOAT_EQ(2.0f, aabb.height);
  EXPECT_FLOAT_EQ(2.0f, aabb.depth);

  AABB aabbCopy(aabb);

  EXPECT_FLOAT_EQ(aabb.minX, aabbCopy.minX);
  EXPECT_FLOAT_EQ(aabb.minY, aabbCopy.minY);
  EXPECT_FLOAT_EQ(aabb.minZ, aabbCopy.minZ);
  EXPECT_FLOAT_EQ(aabb.maxX, aabbCopy.maxX);
  EXPECT_FLOAT_EQ(aabb.maxY, aabbCopy.maxY);
  EXPECT_FLOAT_EQ(aabb.maxZ, aabbCopy.maxZ);
  EXPECT_FLOAT_EQ(aabb.width, aabbCopy.width);
  EXPECT_FLOAT_EQ(aabb.height, aabbCopy.height);
  EXPECT_FLOAT_EQ(aabb.depth, aabbCopy.depth);

  Vector3 point(1.0f, 2.0f, 3.0f);
  Vector3 dimensions(12.0f, 24.0f, 36.0f);
  AABB b1(point, dimensions);

  EXPECT_FLOAT_EQ(point.x, b1.minX);
  EXPECT_FLOAT_EQ(point.y, b1.minY);
  EXPECT_FLOAT_EQ(point.z, b1.minZ);
  EXPECT_FLOAT_EQ(point.x + dimensions.x, b1.maxX);
  EXPECT_FLOAT_EQ(point.y + dimensions.y, b1.maxY);
  EXPECT_FLOAT_EQ(point.z + dimensions.z, b1.maxZ);
  EXPECT_FLOAT_EQ(dimensions.x, b1.width);
  EXPECT_FLOAT_EQ(dimensions.y, b1.height);
  EXPECT_FLOAT_EQ(dimensions.z, b1.depth);

}

TEST(AABB_TESTS, IsPointInsideAABBTests)
{
  AABB box(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);
  
  Vector3 inPoint(8.0f, 3.0f, 9.0f);
  EXPECT_TRUE(IsPointInsideAABB(inPoint, box));

  Vector3 outPoint(8.0f, 3.0f, 12.0f);
  EXPECT_FALSE(IsPointInsideAABB(outPoint, box));
}

TEST(AABB_TESTS, DoesAABBIntersectTests)
{
  AABB b1(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);
  AABB b2(5.0f, 3.0f, 3.0f, 5.0f, 9.0f, 12.0f);

  EXPECT_TRUE(DoesAABBIntersect(b1,b2));

  AABB b3(b1);
  AABB b4(12.0f, 11.0f, 19.0f, 24.0f, 22.0f, 38.0f);

  EXPECT_FALSE(DoesAABBIntersect(b3, b4));
}

