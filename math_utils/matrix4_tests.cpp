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

  Matrix4 scaleMat = MathUtils::makeScaleMatrix(4.0f);
  EXPECT_FLOAT_EQ(4.0f, scaleMat[0]);
  EXPECT_FLOAT_EQ(4.0f, scaleMat[5]);
  EXPECT_FLOAT_EQ(4.0f, scaleMat[10]);
  EXPECT_FLOAT_EQ(1.0f, scaleMat[15]);

  Matrix4 scaleMat2(MathUtils::makeScaleMatrix(3.0f));
  EXPECT_FLOAT_EQ(3.0f, scaleMat2[0]);
  EXPECT_FLOAT_EQ(3.0f, scaleMat2[5]);
  EXPECT_FLOAT_EQ(3.0f, scaleMat2[10]);
  EXPECT_FLOAT_EQ(1.0f, scaleMat2[15]);

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
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 2.0f, 3.0f, 1.0f
  });

  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(expMat[i], transMat[i]);
  }

  Matrix4 viewMat(MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, -3.0f)));
  Matrix4 expMat2({
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, -3.0f, 1.0f
  });
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(expMat2[i], viewMat[i]);
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
    56.0f, 64.0f, 72.0f, 80.0f,
    56.0f, 64.0f, 72.0f, 80.0f,
    56.0f, 64.0f, 72.0f, 80.0f,
    56.0f, 64.0f, 72.0f, 80.0f
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
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    2.0f, 3.0f, 5.0f, 1.0f
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
  float l = -1.0f;
  float r = 1.0f;
  float b = -1.0f;
  float t = 1.0f;
  float n = 0.1f;
  float f = 100.0f;

  Matrix4 expMat({
    (2.0f * n) / (r - l), 0.0f, (r + l) / (r - l), 0.0f,
    0.0f, (2.0f * n) / (t - b), (t + b) / (t - b), 0.0f,
    0.0f, 0.0f, -1.0f * (f + n) / (f - n), -1.0f * (2.0f * f * n) / (f - n),
    0.0f, 0.0f, -1.0f, 0.0f
  });

  // row major to column major order conversion
  expMat.transpose();

  Matrix4 actMat(MathUtils::createPerspectiveMatrix(l, r, b, t, n, f));

  float tolerance = 1e-5f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(expMat[i], actMat[i], tolerance);
  }

  float width = 1920.0f;
  float height = 1080.0f;
  float aspect = width / height;
  float fov = 90.0f;
  float near = 0.1f;
  float far = 100.0f;

  float degToRad = (MathUtils::PI / 180.0f) * fov;
  float vertFov = 1.0f / tan(0.5f * degToRad);
  float horizFov = (1.0f / aspect) * vertFov;

  Matrix4 actMat2 = MathUtils::createPerspectiveMatrix(aspect, fov, near, far);
  Matrix4 expMat2 = Matrix4({
    horizFov, 0.0f, 0.0f, 0.0f,
    0.0f, vertFov, 0.0f, 0.0f,
    0.0f, 0.0f, -(far+near) / (far-near), -1.0f,
    0.0f, 0.0f, -2.0f*(far*near) / (far-near), 0.0f
  });

  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(expMat2[i], actMat2[i], tolerance);
  }

  /* Assertion Tests */
  EXPECT_DEATH(MathUtils::createPerspectiveMatrix(0.0f, fov, near, far), "aspect > 0\\.0f");
  EXPECT_DEATH(MathUtils::createPerspectiveMatrix(aspect, -5.0f, near, far), "fov > 0\\.0f && fov < 180\\.0f");
  EXPECT_DEATH(MathUtils::createPerspectiveMatrix(aspect, 181.0f, near, far), "fov > 0\\.0f && fov < 180\\.0f");
  EXPECT_DEATH(MathUtils::createPerspectiveMatrix(aspect, fov, 0.0f, far), "n > 0\\.0f");
  EXPECT_DEATH(MathUtils::createPerspectiveMatrix(aspect, fov, 5.0f, 1.0f), "f > 0\\.0f && f > n");
}

TEST(Matrix4Tests, OrthographicMatrices)
{
  Matrix4 expMat({
    2.0f / (1.0f - -1.0f), 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f / (1.0f - -1.0f), 0.0f, 0.0f,
    0.0f, 0.0f, -2.0f / (100.0f - 0.1f), 0.0f,
    -1.0f * (1.0f + -1.0f) / (1.0f - -1.0f), -1.0f * (1.0f + -1.0f) / (1.0f - -1.0f), -1 * (100.0f + 0.1f) / (100.0f - 0.1f), 1.0f
  });

  Matrix4 actualMat = MathUtils::createOrthographicMatrix(
    -1.0f, 1.0f,
    -1.0f, 1.0f,
    0.1f, 100.0f
  );

  float tolerance = 1e-5f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(expMat[i], actualMat[i], tolerance);
  }
}

TEST(Matrix4Tests, Determinants)
{
  Matrix4 mat({
    1, 2, 3, 4,
    3, 2, 5, 1,
    2, 9, 3, 1,
    2, 1, 3, 1
  });

  // plugin matrix into wolframalpha calculator online to get determinant here
  float expAnswer = 40.0f;

  float actualAnswer = mat.determinant();
  EXPECT_FLOAT_EQ(expAnswer, actualAnswer);
}

TEST(Matrix4Tests, Adjugates)
{
  Matrix4 mat({
    1, 2, 3, 4,
    3, 2, 5, 1,
    2, 9, 3, 1,
    2, 1, 3, 1
  });

  Matrix4 answer({
    -16, -72, 5, 131,
    0, 0, 5, -5,
    8, 56, -5, -83,
    8, -24, 0, 32
  });

  Matrix4 actual = mat.adjugate();
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_FLOAT_EQ(answer[i], actual[i]);
  }
}

TEST(Matrix4Tests, InverseMatrix)
{
  Matrix4 mat({
    1, 2, 3, 4,
    3, 2, 5, 1,
    2, 9, 3, 1,
    2, 1, 3, 1
  });

  Matrix4 answer({
    -0.4, -1.8, 0.125, 3.275,
    0, 0, 0.125, -0.125,
    0.2, 1.4, -0.125, -2.075,
    0.2, -0.6, 0, 0.8
  });

  Matrix4 actual = mat.calculateInverse();
  float tolerance =1e-5f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(answer[i], actual[i], tolerance);
  }

  Matrix4 invalidMat({
    0, 0, 0, 0,
    3, 2, 5, 1,
    2, 9, 3, 1,
    2, 1, 3, 1
  });
  EXPECT_DEATH(invalidMat.calculateInverse(), "absDet > 1e-8f");

  // Check Identity Next
  Matrix4 invMat = mat.calculateInverse();
  Matrix4 actualIdentity = mat * invMat;
  
  tolerance = 0.001f;
  for (int i = 0;i < MAT4_SIZE; ++i) {
    EXPECT_NEAR(MAT4_IDENTITY[i], actualIdentity[i], tolerance);
  }
}

TEST(Matrix4Tests, MatrixPerspectiveIntegrationTest)
{
  const float tolerance = 1e-5f;
  const float width = 1280.0f;
  const float height = 800.0f;
  float x = width / 4.0f;
  float y = height / 4.0f;
  Vector3 ndcCoordinates = MathUtils::convertFromScreenSpaceToNDC(x, y, width, height);
  // -1 is set here because this is for projecting a ray towards the near clipping plane
  ndcCoordinates.z = -1.0f;

  EXPECT_NEAR(-0.5f, ndcCoordinates.x, tolerance);
  EXPECT_NEAR(0.5f, ndcCoordinates.y, tolerance);

  float fovDegrees = 90.0f;
  float aspect = width / height;
  float near = 0.1f;
  float far = 1000.0f;
  Matrix4 projectionMatrix(MathUtils::createPerspectiveMatrix(aspect, fovDegrees, near, far));

  Matrix4 inverse = projectionMatrix.calculateInverse();
  std::vector<float> viewSpaceRaw(inverse.matrixVertMult(ndcCoordinates));

  EXPECT_TRUE(viewSpaceRaw.size() == 4);
  EXPECT_TRUE(viewSpaceRaw[3] != 0.0f);

  // the w unprojection must be 10 because 10 is the inverse of 0.1 which is the near clipping plane distance value
  float w = viewSpaceRaw[3];
  EXPECT_NEAR(10.0f, w, tolerance);
}

TEST(Matrix4Tests, VerifyScreenToWorldCoordinates)
{
  float mouseX = 120.0f, mouseY = 653.0f;
  float distFromCamera = 10.0f;
  float width = 1280.0f, height = 800.0f;
  float near = 0.1f, far = 100.0f;

  float aspect = width / height;
  float fovDegrees = 90.0f;
  Matrix4 projMat(MathUtils::createPerspectiveMatrix(aspect, fovDegrees, near, far));
  Matrix4 viewMat(MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, -1.0f)));

  Vector3 worldCoord = MathUtils::screenSpaceToWorldSpace(
    mouseX, mouseY, distFromCamera,
    width, height, near, far,
    projMat, viewMat, true
  );

  // calculate the reverse to verify screenSpaceToWorldSpace works as intended
  std::vector<float> viewCoord = viewMat.matrixVertMult(worldCoord);
  Vector3 vC(viewCoord[0], viewCoord[1], viewCoord[2]);
  std::vector<float> clipCoord = projMat.matrixVertMult(vC);

  // convert from clip to NDC
  Vector3 ndcCoord(clipCoord[0], clipCoord[1], clipCoord[2]);
  ndcCoord *= (1.0f / clipCoord[3]);

  // convert from NDC to screen
  float screenX = (width * (ndcCoord.x + 1.0f)) / 2.0f;
  float screenY = ((1.0f - ndcCoord.y) / 2.0f) * height;
  
  // due to floating point inprecisions, checking if its approx the original mouse values should be an acceptable check here!
  const float tolerance = 0.001f;
  EXPECT_NEAR(mouseX, screenX, tolerance);
  EXPECT_NEAR(mouseY, screenY, tolerance);
}
