#include <gtest/gtest.h>

#include "vector3.h"
using namespace MathUtils;

TEST(Vector3Tests, Constructors)
{
  Vector3 uninitialized;
  EXPECT_FLOAT_EQ(0.0f, uninitialized.x);
  EXPECT_FLOAT_EQ(0.0f, uninitialized.y);
  EXPECT_FLOAT_EQ(0.0f, uninitialized.z);

  Vector3 same(5.0f);
  EXPECT_FLOAT_EQ(same.x, same.y);
  EXPECT_FLOAT_EQ(same.y, same.z);

  Vector3 diff(1.0f, 2.0f, 3.0f);
  EXPECT_FLOAT_EQ(1.0f, diff.x);
  EXPECT_FLOAT_EQ(2.0f, diff.y);
  EXPECT_FLOAT_EQ(3.0f, diff.z);

  uninitialized.x = 10.0f;
  uninitialized.y = 20.0f;
  uninitialized.z = 30.0f;
  Vector3 copyVec(uninitialized);
  EXPECT_FLOAT_EQ(uninitialized.x, copyVec.x);
  EXPECT_FLOAT_EQ(uninitialized.y, copyVec.y);
  EXPECT_FLOAT_EQ(uninitialized.z, copyVec.z);
}

TEST(Vector3Tests, Addition)
{
  Vector3 a{ 3.0f, 4.0f, 5.0f };
  Vector3 b{ 1.0f, 8.0f, 9.0f };
  Vector3 c = a + b;
  EXPECT_FLOAT_EQ(4.0f, c.x);
  EXPECT_FLOAT_EQ(12.0f, c.y);
  EXPECT_FLOAT_EQ(14.0f, c.z);

  Vector3 d(c);
  d += a;
  EXPECT_FLOAT_EQ(7.0f, d.x);
  EXPECT_FLOAT_EQ(16.0f, d.y);
  EXPECT_FLOAT_EQ(19.0f, d.z);
  EXPECT_FLOAT_EQ(3.0f, a.x);
  EXPECT_FLOAT_EQ(4.0f, a.y);
  EXPECT_FLOAT_EQ(5.0f, a.z);

  Vector3 expectedVec(c);
  c += c;
  EXPECT_FLOAT_EQ(2.0f * expectedVec.x, c.x);
  EXPECT_FLOAT_EQ(2.0f * expectedVec.y, c.y);
  EXPECT_FLOAT_EQ(2.0f * expectedVec.z, c.z);
}

TEST(Vector3Tests, Subtraction)
{
  Vector3 a(100.0f, 80.0f, 60.0f);
  Vector3 b(50.0f, 20.0f, 90.0f);
  Vector3 c = a - b;

  EXPECT_FLOAT_EQ(50.0f, c.x);
  EXPECT_FLOAT_EQ(60.0f, c.y);
  EXPECT_FLOAT_EQ(-30.0f, c.z);

  c -= c;
  EXPECT_FLOAT_EQ(0.0f, c.x);
  EXPECT_FLOAT_EQ(0.0f, c.y);
  EXPECT_FLOAT_EQ(0.0f, c.z);

  a -= b;
  EXPECT_FLOAT_EQ(50.0f, a.x);
  EXPECT_FLOAT_EQ(60.0f, a.y);
  EXPECT_FLOAT_EQ(-30.0f, a.z);
  EXPECT_FLOAT_EQ(50.0f, b.x);
  EXPECT_FLOAT_EQ(20.0f, b.y);
  EXPECT_FLOAT_EQ(90.0f, b.z);
}

TEST(Vector3Tests, ScalarMultiplication)
{
  float scalar = 2.0f;
  Vector3 a(3.0f, 6.0f, 9.0f);
  a = a * scalar;
  EXPECT_FLOAT_EQ(scalar * 3.0f, a.x);
  EXPECT_FLOAT_EQ(scalar * 6.0f, a.y);
  EXPECT_FLOAT_EQ(scalar * 9.0f, a.z);

  Vector3 b(25.0f, 30.0f, 70.0f);
  b = 2 * b;
  EXPECT_FLOAT_EQ(scalar * 25.0f, b.x);
  EXPECT_FLOAT_EQ(scalar * 30.0f, b.y);
  EXPECT_FLOAT_EQ(scalar * 70.0f, b.z);

  Vector3 c(3.0f, 3.0f, 3.0f);
  c *= 4.0f;
  EXPECT_FLOAT_EQ(12.0f, c.x);
  EXPECT_FLOAT_EQ(12.0f, c.y);
  EXPECT_FLOAT_EQ(12.0f, c.z);
}

TEST(Vector3Tests, ScalarDivision)
{
  Vector3 a(16.0f, 8.0f, 4.0f);
  Vector3 b = a / 2.0f;
  EXPECT_FLOAT_EQ(8.0f, b.x);
  EXPECT_FLOAT_EQ(4.0f, b.y);
  EXPECT_FLOAT_EQ(2.0f, b.z);
  EXPECT_FLOAT_EQ(16.0f, a.x);
  EXPECT_FLOAT_EQ(8.0f, a.y);
  EXPECT_FLOAT_EQ(4.0f, a.z);

  a /= 2.0f;
  EXPECT_FLOAT_EQ(8.0f, a.x);
  EXPECT_FLOAT_EQ(4.0f, a.y);
  EXPECT_FLOAT_EQ(2.0f, a.z);

  /* Division by Zero Assertion Checks */
  EXPECT_DEATH(a = a / 0.0f, "scalar != 0\\.0f");
  EXPECT_DEATH(b /= 0.0f, "scalar != 0\\.0f");
}

TEST(Vector3Tests, DotProduct)
{
  Vector3 a(5.0f, 10.0f, 15.0f);
  Vector3 b(2.0f, 3.0f, 4.0f);

  float ans1 = a.dot(a);
  EXPECT_FLOAT_EQ(350.0f,ans1);

  float ans2 = a.dot(b);
  float ans3 = b.dot(a);
  EXPECT_FLOAT_EQ(ans2, ans3);
}

TEST(Vector3Tests, CrossProduct)
{
  Vector3 a(5.0f, 10.0f, 15.0f);
  Vector3 b(2.0f, 3.0f, 4.0f);

  Vector3 c = a.cross(b);
  Vector3 d = b.cross(a);

  EXPECT_FLOAT_EQ(-5.0f, c.x);
  EXPECT_FLOAT_EQ(10.0f, c.y);
  EXPECT_FLOAT_EQ(-5.0f, c.z);
  EXPECT_FLOAT_EQ(5.0f, d.x);
  EXPECT_FLOAT_EQ(-10.0f, d.y);
  EXPECT_FLOAT_EQ(5.0f, d.z);

  // cross product not commutative
  EXPECT_NE(c.x, d.x);
  EXPECT_NE(c.y, d.y);
  EXPECT_NE(c.z, d.z);

}

TEST(Vector3Tests, SqrMagnitude)
{
  Vector3 a(3.0f, 6.0f, 9.0f);
  float a1 = a.dot(a);
  float a2 = a.sqrMagnitude();
  EXPECT_FLOAT_EQ(126.0f, a1);
  EXPECT_FLOAT_EQ(a1, a2);
}

TEST(Vector3Tests, Magnitude)
{
  Vector3 a(3.0f, 4.0f, 5.0f);
  float a1 = a.magnitude();
  EXPECT_FLOAT_EQ(sqrtf(3.0f * 3.0f + 4.0f * 4.0f + 5.0f * 5.0f), a1);
}

TEST(Vector3Tests, Normalize)
{
  Vector3 a(3.0f, 4.0f, 5.0f);
  a.normalize();
  EXPECT_FLOAT_EQ(1.0f, a.magnitude());
}