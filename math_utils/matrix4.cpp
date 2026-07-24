#include <assert.h>
#include <cmath>

#include "matrix4.h"
#include "vector3.h"

using namespace MathUtils;

Matrix4 Matrix4::operator*(const Matrix4& rhs) const {
  Matrix4 output;
  int i = 0;
  for (int r1 = 0; r1 < MAT4_DIM; ++r1) {
    for (int c2 = 0; c2 < MAT4_DIM; ++c2) {
      float dotProduct = 0.0f;
      for (int rc = 0; rc < MAT4_DIM; ++rc) {
        int rowMajorIndex1 = r1 * MAT4_DIM + rc;
        int rowMajorIndex2 = rc * MAT4_DIM + c2;
        dotProduct += cells[rowMajorIndex1] * rhs.cells[rowMajorIndex2];
      }

      output.cells[i] = dotProduct;
      ++i;
    }
  }

  return output;
}

Matrix4 Matrix4::operator*=(const Matrix4& rhs) {
  *this = *this * rhs;
  return *this;
}

Vector3 Matrix4::operator*(const Vector3& rhs) const {
  float temp[4] { rhs.x, rhs.y, rhs.z, 1.0f };
  float midAns[4] { 0.0f, 0.0f, 0.0f, 0.0f };
  int j = 0;

  for (int i = 0; i < MAT4_SIZE; ++i) {
    midAns[j] += cells[i] * temp[i % MAT4_DIM];

    if (i % MAT4_DIM == 3) {
      ++j;
    }
  }

  // drop the extra 4th dimension here as its not needed in the final computation
  return Vector3(midAns[0], midAns[1], midAns[2]);
}

float Matrix4::operator[](int i) const {
  assert(i >= 0 && i < MAT4_SIZE);
  return cells[i];
}

void Matrix4::transpose() {
  for (int r = 0; r < MAT4_DIM; ++r) {
    for (int c = r; c < MAT4_DIM; ++c) {
      int rowMajorIndex = r * MAT4_DIM + c;
      int colMajorIndex = c * MAT4_DIM + r;
      
      float temp = cells[rowMajorIndex];
      cells[rowMajorIndex] = cells[colMajorIndex];
      cells[colMajorIndex] = temp;
    }
  }
}

Matrix4 Matrix4::operator*(const float scalar) const {
  float temp[MAT4_SIZE] {};
  for (int i = 0; i < MAT4_SIZE; ++i) {
    temp[i] = scalar * cells[i];
  }
  return Matrix4(temp);
}

Matrix4 MathUtils::operator*(const float scalar, const Matrix4& rhs) {
  return rhs * scalar;
}

Matrix4 Matrix4::operator*=(const float scalar) {
  *this = *this * scalar;
  return *this;
}

Matrix4 Matrix4::operator+(const Matrix4& rhs) const {
  float temp[MAT4_SIZE] {};
  for (int i = 0;i < MAT4_SIZE; ++i) {
    temp[i] = cells[i] + rhs.cells[i];
  }

  return Matrix4(temp);
}

Matrix4 MathUtils::makeTranslationMatrix(const Vector3& offset) {
  Matrix4 translate({
    0.0f, 0.0f, 0.0f, offset.x,
    0.0f, 0.0f, 0.0f, offset.y,
    0.0f, 0.0f, 0.0f, offset.z,
    0.0f, 0.0f, 0.0f, 0.0f
  });

  return Matrix4(MAT4_IDENTITY + translate);
}

/* 
  See 5.1.3 3D Rotation about an arbirtrary axis:
  https://gamemath.com/book/matrixtransforms.html
*/
Matrix4 MathUtils::rotate(const Matrix4& m, const float degreesRadians, const Vector3& normalizedAxis) {
  const Vector3 n(normalizedAxis);
  float d = degreesRadians;
  float cosT = (float)std::cos(d);
  float oneMinusCos = 1.0f - cosT;
  float sinT = (float)std::sin(d);

  Matrix4 rotationMatrix({
    n.x * n.x * oneMinusCos + cosT, n.x * n.y * oneMinusCos + n.z * sinT, n.x * n.z * oneMinusCos - n.y * sinT, 0.0f,
    n.x * n.y * oneMinusCos - n.z * sinT, n.y * n.y * oneMinusCos + cosT, n.y * n.z * oneMinusCos + n.x * sinT, 0.0f,
    n.x * n.z * oneMinusCos + n.y * sinT, n.y * n.z * oneMinusCos - n.x * sinT, n.z * n.z * oneMinusCos + cosT, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  });

  return m * rotationMatrix;
}

Vector3 MathUtils::rotate(const Matrix4& m, const Vector3& v) {
  return m * v;
}