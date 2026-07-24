#include <gtest/gtest.h>
#include <cmath>

#include "matrix4.h"
#include "vector3.h"
using namespace MathUtils;

TEST(Matrix4Tests, IdentityMatrix)
{
  const float arr[MAT4_SIZE] {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  for (int i = 0;i < MAT4_SIZE; ++i) {
    float c1 = arr[i];
    float c2 = MAT4_IDENTITY[i];
    EXPECT_FLOAT_EQ(arr[i], MAT4_IDENTITY[i]);
  }
}

TEST(Matrix4Tests, ScaleMatrices)
{
  Matrix4 mat({
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  Matrix4 mat1(mat * 2.0f);
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(2.0f * mat[i], mat1[i]);
  }

  Matrix4 mat2 = 2.0f * mat;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(2.0f * mat[i], mat2[i]);
  }

  mat2 *= 2.0f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(mat[i] * 4.0f, mat2[i]);
  }

}

TEST(Matrix4Tests, MatrixAdd)
{
  Matrix4 mat({
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  Matrix4 sumMat(mat + mat);
  for (int i = 0; i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(mat[i] + mat[i], sumMat[i]);
  }

  Matrix4 a({
    2.0f, 4.0f, 6.0f, 8.0f,
    10.0f, 12.0f, 14.0f, 16.0f,
    18.0f, 20.0f, 22.0f, 24.0f,
    26.0f, 28.0f, 30.0f, 32.0f
  });

  Matrix4 b({
    1.0f, 3.0f, 5.0f, 7.0f,
    9.0f, 11.0f, 13.0f, 15.0f,
    17.0f, 21.0f, 23.0f, 25.0f,
    27.0f, 29.0f, 31.0f, 33.0f
  });

  Matrix4 sum1 = a + b;
  Matrix4 sum2 = b + a;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(a[i] + b[i], sum1[i]);
    EXPECT_FLOAT_EQ(sum1[i], sum2[i]);
  }
}

TEST(Matrix4Tests, TranslationMatrices)
{
  Matrix4 transMat = MathUtils::makeTranslationMatrix(Vector3(1.0f, 2.0f, 3.0f));

  Matrix4 expMat({
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 2.0f,
    0.0f, 0.0f, 1.0f, 3.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  });

  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(expMat[i], transMat[i]);
  }
}

TEST(Matrix4Tests, RotationMatrices)
{
  float radians = 90.0f * (MathUtils::PI / 180.0f);
  Vector3 rotationAxis(1.0f, 0.0f, 0.0f);
  Matrix4 rotatedMat = MathUtils::rotate(MAT4_IDENTITY, radians, rotationAxis);

  Matrix4 expMat({
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  });

  const float tolerance = 1e-5f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(rotatedMat[i], expMat[i], tolerance);
  }

  Vector3 v(0.0f, 0.0f, 1.0f);
  // NOTE: need to transpose rotateMat in order to maintain right handed coordinates for OpenGL
  rotatedMat.transpose();
  Vector3 vOutput = MathUtils::rotate(rotatedMat, v);
  Vector3 expVector(0.0f, -1.0f, 0.0f);
  EXPECT_NEAR(vOutput.x, expVector.x, tolerance);
  EXPECT_NEAR(vOutput.y, expVector.y, tolerance);
  EXPECT_NEAR(vOutput.z, expVector.z, tolerance);
}

TEST(Matrix4Tests, MatrixMultiplication)
{
  Matrix4 mat({
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  Matrix4 mat2(mat * MAT4_IDENTITY);

  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(mat[i], mat2[i]);
  }

  Matrix4 mat3 = mat * MAT4_IDENTITY;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(mat[i], mat3[i]);
  }

  Matrix4 mat4 = mat * mat;
  Matrix4 expMat({
    90.0f, 100.0f, 110.0f, 120.0f,
    202.0f, 228.0f, 254.0f, 280.0f,
    314.0f, 356.0f, 398.0f, 440.0f,
    426.0f, 484.0f, 542.0f, 600.0f
  });

  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(expMat[i], mat4[i]);
  }

  Matrix4 mat5({
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  Matrix4 mat6({
    1.0f, 2.0f, 8.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  // not commutative test
  Matrix4 mat7(mat5 * mat6);
  Matrix4 mat8 = mat6 * mat5;
  bool atLeastOneDoesNotMatch = false;
  for (int i = 0; i < MAT4_SIZE; ++i) {
    if (mat7[i] != mat8[i]) {
      atLeastOneDoesNotMatch = true;
      break;
    }
  }
  EXPECT_TRUE(atLeastOneDoesNotMatch);

  Matrix4 mat9(mat);
  Matrix4 mat10({
    2.0f, 2.0f, 2.0f, 2.0f,
    2.0f, 2.0f, 2.0f, 2.0f,
    2.0f, 2.0f, 2.0f, 2.0f,
    2.0f, 2.0f, 2.0f, 2.0f
  });

  Matrix4 expMat2({
    20.0f, 20.0f, 20.0f, 20.0f,
    52.0f, 52.0f, 52.0f, 52.0f,
    84.0f, 84.0f, 84.0f, 84.0f,
    116.0f, 116.0f, 116.0f, 116.0f
  });

  mat9 *= MAT4_IDENTITY;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(mat[i], mat9[i]);
  }

  mat9 *= mat10;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(expMat2[i], mat9[i]);
  }
}

TEST(Matrix4Tests, MatrixVectorMultiplication)
{
  Vector3 a(1.0f, 2.0f, 3.0f);
  Vector3 a1 = MAT4_IDENTITY * a;
  EXPECT_FLOAT_EQ(a.x, a1.x);
  EXPECT_FLOAT_EQ(a.y, a1.y);
  EXPECT_FLOAT_EQ(a.z, a1.z);

  Matrix4 transMatrix({
    1.0f, 0.0f, 0.0f, 2.0f, // 3
    0.0f, 1.0f, 0.0f, 3.0f, // 5
    0.0f, 0.0f, 1.0f, 5.0f, // 8
    0.0f, 0.0f, 0.0f, 1.0f  // 1
  });

  Vector3 a2(transMatrix * a);
  EXPECT_FLOAT_EQ(2.0f + a.x, a2.x);
  EXPECT_FLOAT_EQ(3.0f + a.y, a2.y);
  EXPECT_FLOAT_EQ(5.0F + a.z, a2.z);
}

TEST(Matrix4Tests, Transpose)
{
  Matrix4 m({
    1.0f, 2.0f, 3.0f, 4.0f,
    5.0f, 6.0f, 7.0f, 8.0f,
    9.0f, 10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f, 16.0f
  });

  Matrix4 m2({
    1.0f, 5.0f, 9.0f, 13.0f,
    2.0f, 6.0f, 10.0f, 14.0f,
    3.0f, 7.0f, 11.0f, 15.0f,
    4.0f, 8.0f, 12.0f, 16.0f
  });

  m.transpose();
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(m2[i], m[i]);
  }

  Matrix4 m3(MAT4_IDENTITY);
  m3.transpose();
  for (int i = 0; i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(MAT4_IDENTITY[i], m3[i]);
  }
}

TEST(Matrix4Tests, PerspectiveMatrices)
{

}

TEST(Matrix4Tests, OrthographicMatrices)
{

}
