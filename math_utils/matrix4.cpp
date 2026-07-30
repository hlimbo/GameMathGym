#include <assert.h>
#include <cmath>

#include "vector3.h"
#include "matrix4.h"

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

std::vector<float> Matrix4::matrixVertMult(const Vector3& rhs) const {
  float temp[4] { rhs.x, rhs.y, rhs.z, 1.0f };
  float midAns[4] { 0.0f, 0.0f, 0.0f, 0.0f };
  int j = 0;

  for (int i = 0; i < MAT4_SIZE; ++i) {
    midAns[j] += cells[i] * temp[i % MAT4_DIM];

    if (i % MAT4_DIM == 3) {
      ++j;
    }
  }

  return std::vector<float>({midAns[0], midAns[1], midAns[2], midAns[3]});
}

Vector3 Matrix4::operator*(const Vector3& rhs) const {
  std::vector ans(matrixVertMult(rhs));
  // drop the extra 4th dimension here as its not needed in the final computation
  return Vector3(ans[0], ans[1], ans[2]);
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

float Matrix4::determinant() const {
  // https://semath.info/src/inverse-cofactor-ex4.html
  float det = 
    cells[0]*cells[5]*cells[10]*cells[15] + cells[0]*cells[6]*cells[11]*cells[13] + cells[0]*cells[7]*cells[9]*cells[14]
    - cells[0]*cells[7]*cells[10]*cells[13] - cells[0]*cells[6]*cells[9]*cells[15] - cells[0]*cells[5]*cells[11]*cells[14]
    - cells[1]*cells[4]*cells[10]*cells[15] - cells[2]*cells[4]*cells[11]*cells[13] - cells[3]*cells[4]*cells[9]*cells[14]
    + cells[3]*cells[4]*cells[10]*cells[13] + cells[2]*cells[4]*cells[9]*cells[15] + cells[1]*cells[4]*cells[11]*cells[14]
    + cells[1]*cells[6]*cells[8]*cells[15] + cells[2]*cells[7]*cells[8]*cells[13] + cells[3]*cells[5]*cells[8]*cells[14]
    - cells[3]*cells[6]*cells[8]*cells[13] - cells[2]*cells[5]*cells[8]*cells[15] - cells[1]*cells[7]*cells[8]*cells[14]
    - cells[1]*cells[6]*cells[11]*cells[12] - cells[2]*cells[7]*cells[9]*cells[12] - cells[3]*cells[5]*cells[10]*cells[12]
    + cells[3]*cells[6]*cells[9]*cells[12] + cells[2]*cells[5]*cells[11]*cells[12] + cells[1]*cells[7]*cells[10]*cells[12];

  return det;
}

Matrix4 Matrix4::adjugate() const {
  Matrix4 o;

  // https://semath.info/src/inverse-cofactor-ex4.html
  float a11 = cells[0], a12 = cells[1], a13 = cells[2], a14 = cells[3];
  float a21 = cells[4], a22 = cells[5], a23 = cells[6], a24 = cells[7];
  float a31 = cells[8], a32 = cells[9], a33 = cells[10], a34 = cells[11];
  float a41 = cells[12], a42 = cells[13], a43 = cells[14], a44 = cells[15];

  o.cells[0] = a22*a33*a44 + a23*a34*a42 + a24*a32*a43 - a24*a33*a42 - a23*a32*a44 - a22*a34*a43;
  o.cells[1] = -a12*a33*a44 - a13*a34*a42 - a14*a32*a43 + a14*a33*a42 + a13*a32*a44 + a12*a34*a43;
  o.cells[2] = a12*a23*a44 + a13*a24*a42 + a14*a22*a43 - a14*a23*a42 - a13*a22*a44 - a12*a24*a43;
  o.cells[3] = -a12*a23*a34 - a13*a24*a32 - a14*a22*a33 + a14*a23*a32 + a13*a22*a34 + a12*a24*a33;

  o.cells[4] = -a21*a33*a44 - a23*a34*a41 - a24*a31*a43 + a24*a33*a41 + a23*a31*a44 + a21*a34*a43;
  o.cells[5] = a11*a33*a44 + a13*a34*a41 + a14*a31*a43 - a14*a33*a41 - a13*a31*a44 - a11*a34*a43;
  o.cells[6] = -a11*a23*a44 - a13*a24*a41 - a14*a21*a43 + a14*a23*a41 + a13*a21*a44 + a11*a24*a43;
  o.cells[7] = a11*a23*a34 + a13*a24*a31 + a14*a21*a33 - a14*a23*a31 - a13*a21*a34 - a11*a24*a33;

  o.cells[8] = a21*a32*a44 + a22*a34*a41 + a24*a31*a42 - a24*a32*a41 - a22*a31*a44 - a21*a34*a42;
  o.cells[9] = -a11*a32*a44 - a12*a34*a41 - a14*a31*a42 + a14*a32*a41 + a12*a31*a44 + a11*a34*a42;
  o.cells[10] = a11*a22*a44 + a12*a24*a41 + a14*a21*a42 - a14*a22*a41 - a12*a21*a44 - a11*a24*a42;
  o.cells[11] = -a11*a22*a34 - a12*a24*a31 - a14*a21*a32 + a14*a22*a31 + a12*a21*a34 + a11*a24*a32;

  o.cells[12] = -a21*a32*a43 - a22*a33*a41 - a23*a31*a42 + a23*a32*a41 + a22*a31*a43 + a21*a33*a42;
  o.cells[13] = a11*a32*a43 + a12*a33*a41 + a13*a31*a42 - a13*a32*a41 - a12*a31*a43 - a11*a33*a42;
  o.cells[14] = -a11*a22*a43 - a12*a23*a41 - a13*a21*a42 + a13*a22*a41 + a12*a21*a43 + a11*a23*a42;
  o.cells[15] =  a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a13*a22*a31 - a12*a21*a33 - a11*a23*a32;

  return o;
}

Matrix4 Matrix4::calculateInverse() const {
  float det = determinant();
  assert(det != 0.0f);
  return (1.0f / det) * adjugate();
}

Matrix4 MathUtils::makeTranslationMatrix(const Vector3& offset) {
  Matrix4 translate({
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    offset.x, offset.y, offset.z, 0.0f
  });

  return Matrix4(MAT4_IDENTITY + translate);
}

Matrix4 MathUtils::makeScaleMatrix(float scale) {
  Matrix4 output(MAT4_IDENTITY);
  output.cells[0] *= scale;
  output.cells[5] *= scale;
  output.cells[10] *= scale;

  return output;
}

Matrix4 MathUtils::makeScaleMatrix(const Vector3& scalars) {
  Matrix4 output(MAT4_IDENTITY);
  output.cells[0] *= scalars.x;
  output.cells[5] *= scalars.y;
  output.cells[10] *= scalars.z;

  return output;
}

/* 
  See 5.1.3 3D Rotation about an arbirtrary axis:
  https://gamemath.com/book/matrixtransforms.html
*/
Matrix4 MathUtils::rotate(const Matrix4& m, const float anglesRadians, const Vector3& normalizedAxis) {
  const Vector3 n(normalizedAxis);
  float d = anglesRadians;
  float cosT = (float)std::cos(d);
  float oneMinusCos = 1.0f - cosT;
  float sinT = (float)std::sin(d);

  Matrix4 rotationMatrix({
    // column 0
    n.x * n.x * oneMinusCos + cosT, 
    n.x * n.y * oneMinusCos - n.z * sinT, 
    n.x * n.z * oneMinusCos + n.y * sinT, 
    0.0f,

    // column 1
    n.x * n.y * oneMinusCos + n.z * sinT, 
    n.y * n.y * oneMinusCos + cosT,
    n.y * n.z * oneMinusCos - n.x * sinT,
    0.0f,

    // column 2
    n.x * n.z * oneMinusCos - n.y * sinT,
    n.y * n.z * oneMinusCos + n.x * sinT,
    n.z * n.z * oneMinusCos + cosT,
    0.0f,

    // column 3
    0.0f,
    0.0f,
    0.0f,
    1.0f
  });

  return m * rotationMatrix;
}

Vector3 MathUtils::rotate(const Matrix4& m, const Vector3& v) {
  return m * v;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix//orthographic-projection-matrix.html
// Note: OpenGL only takes in data in column major order!
Matrix4 MathUtils::createOrthographicMatrix(float l, float r, float b, float t, float n, float f) {
  assert(r != l);
  assert(t != b);
  assert(f != b);

  return Matrix4({
    2.0f / (r - l), 0.0f, 0.0f, 0.0f, // column 0
    0.0f, 2.0f / (t - b), 0.0f, 0.0f, // column 1
    0.0f, 0.0f, -2.0f / (f - n), 0.0f, // column 2
    -1.0f * (r + l) / (r - l), -1.0f * (t + b) / (t - b), -1.0f * (f + n) / (f - n), 1.0f // column 3
  });
}

// https://www.songho.ca/opengl/gl_projectionmatrix.html
Matrix4 MathUtils::createPerspectiveMatrix(float l, float r, float b, float t, float n, float f) {
  return Matrix4({
    (2.0f * n) / (r - l), 0.0f, 0.0f, 0.0f, // column 0 (x basis)
    0.0f, (2.0f * n) / (t - b), 0.0f, 0.0f, // column 1 (y basis)
    (r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1.0f, // column 2 (z basis & frustum shifts)
    0.0f, 0.0f, -(2.0f * f * n) / (f - n), 0.0f, // column 3 (translation / w mapping... nonzero formula is the Z perspective translation)
  });
}

// https://www.songho.ca/opengl/gl_projectionmatrix.html
// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html
// https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/WebGL_model_view_projection#model_transform
Matrix4 MathUtils::createPerspectiveMatrix(float aspect, float fov, float n, float f) {
  assert(fov > 0.0f && fov < 180.0f);
  assert(aspect > 0.0f);
  assert(n > 0.0f);
  assert(f > 0.0f && f > n);
  float degToRad = (MathUtils::PI / 180.0f) * fov;
  float vertFov = 1.0f / tan(0.5f * degToRad);
  float horizFov = (1.0f / aspect) * (vertFov); 
  float zDepth1 = -(f+n) / (f-n);
  float zDepth2 = (-2.0f*f*n) / (f-n);

  // column 0 (x horizontal FOV)
  // column 1 (y vertical FOV)
  // column 2 (used for z-depth and clipping tests)
  // column 3 (used to preserve original depth information for z)
  return Matrix4({
    horizFov,   0.0f,     0.0f,       0.0f, 
    0.0f,       vertFov,  0.0f,       0.0f,
    0.0f,       0.0f,     zDepth1,   -1.0f,
    0.0f,       0.0f,     zDepth2,    0.0f
  });
}

// Column Major order
Matrix4 MathUtils::lookAt(const Vector3& srcPosition, const Vector3& targetPosition, const Vector3& up) {
  Vector3 forwardBasis(targetPosition - srcPosition);
  forwardBasis.normalize();

  Vector3 rightBasis(forwardBasis.cross(up));
  rightBasis.normalize();

  Vector3 upBasis(rightBasis.cross(forwardBasis));
  upBasis.normalize();

  // preserve previous translation units assuming no translations are made
  // will need to calculate it later on when I combine movement and looking around with a camera

  return Matrix4({
    rightBasis.x,     rightBasis.y,     rightBasis.z,     0.0f,
    upBasis.x,        upBasis.y,        upBasis.z,        0.0f,
    -forwardBasis.x,  -forwardBasis.y,  -forwardBasis.z,  0.0f,
    0.0f,             0.0f,             0.0f,             1.0f
  });
}

Vector3 MathUtils::screenSpaceToViewSpace(float x, float y, float width, float height, const Matrix4& projectionMatrix) {
  Vector3 ndcCoordinates = MathUtils::convertFromScreenSpaceToNDC(x, y, width, height);

  // Note: inverted matrix needs to be transposed before applying the multiplication because it is in row major form rather than column major order! 
  Matrix4 inverse = projectionMatrix.calculateInverse();
  inverse.transpose();
  std::vector<float> viewSpaceRaw(inverse.matrixVertMult(ndcCoordinates));

  assert(viewSpaceRaw.size() == 4);
  // w component must not be zero!
  assert(viewSpaceRaw[3] != 0.0f);

  // Perspective Divide by w to ensure the coordinates in view space are scaled correctly
  float w = viewSpaceRaw[3];
  Vector3 viewSpaceCoords(viewSpaceRaw[0] / w, viewSpaceRaw[1] / w, viewSpaceRaw[2] / w);

  return viewSpaceCoords;
}