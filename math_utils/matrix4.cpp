#include <assert.h>
#include <cmath>

#include "vector3.h"
#include "matrix4.h"

using namespace MathUtils;

Matrix4 Matrix4::operator*(const Matrix4& rhs) const {
  Matrix4 output;
  int i = 0;
  for (int c2 = 0; c2 < MAT4_DIM; ++c2) {
    for (int r1 = 0; r1 < MAT4_DIM; ++r1) {
      float dotProduct = 0.0f;
      for (int rc = 0; rc < MAT4_DIM; ++rc) {
        int colMajorIndex1 = rc * MAT4_DIM + r1;
        int colMajorIndex2 = c2 * MAT4_DIM + rc;
        
        dotProduct += cells[colMajorIndex1] * rhs.cells[colMajorIndex2];

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

  for (int i = 0;i < MAT4_DIM; ++i) {
    midAns[i] = 
      cells[i + MAT4_DIM * 0] * temp[0] +
      cells[i + MAT4_DIM * 1] * temp[1] +
      cells[i + MAT4_DIM * 2] * temp[2] +
      cells[i + MAT4_DIM * 3] * temp[3];
  }

  return std::vector<float>({midAns[0], midAns[1], midAns[2], midAns[3]});
}

std::vector<float> Matrix4::matrixVertMult(const Vector3& rhs, float w) const {
  float temp[4] { rhs.x, rhs.y, rhs.z, w };
  float midAns[4] { 0.0f, 0.0f, 0.0f, 0.0f };

  for (int i = 0;i < MAT4_DIM; ++i) {
    midAns[i] = 
      cells[i + MAT4_DIM * 0] * temp[0] +
      cells[i + MAT4_DIM * 1] * temp[1] +
      cells[i + MAT4_DIM * 2] * temp[2] +
      cells[i + MAT4_DIM * 3] * temp[3];
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

Matrix4 Matrix4::adjugate() const {
    Matrix4 o;

    // Column 0 (indices 0, 1, 2, 3)
    float a11 = cells[0],  a21 = cells[1],  a31 = cells[2],  a41 = cells[3];
    // Column 1 (indices 4, 5, 6, 7)
    float a12 = cells[4],  a22 = cells[5],  a32 = cells[6],  a42 = cells[7];
    // Column 2 (indices 8, 9, 10, 11)
    float a13 = cells[8],  a23 = cells[9],  a33 = cells[10], a43 = cells[11];
    // Column 3 (indices 12, 13, 14, 15)
    float a14 = cells[12], a24 = cells[13], a34 = cells[14], a44 = cells[15];

    // First Row of Adjugate (Col 0 of output in Column-Major)
    o.cells[0]  =  a22*(a33*a44 - a34*a43) - a23*(a32*a44 - a34*a42) + a24*(a32*a43 - a33*a42);
    o.cells[4]  = -a12*(a33*a44 - a34*a43) + a13*(a32*a44 - a34*a42) - a14*(a32*a43 - a33*a42);
    o.cells[8]  =  a12*(a23*a44 - a24*a43) - a13*(a22*a44 - a24*a42) + a14*(a22*a43 - a23*a42);
    o.cells[12]  = -a12*(a23*a34 - a24*a33) + a13*(a22*a34 - a24*a32) - a14*(a22*a33 - a23*a32);

    // Second Row of Adjugate (Col 1 of output in Column-Major)
    o.cells[1]  = -a21*(a33*a44 - a34*a43) + a23*(a31*a44 - a34*a41) - a24*(a31*a43 - a33*a41);
    o.cells[5]  =  a11*(a33*a44 - a34*a43) - a13*(a31*a44 - a34*a41) + a14*(a31*a43 - a33*a41);
    o.cells[9]  = -a11*(a23*a44 - a24*a43) + a13*(a21*a44 - a24*a41) - a14*(a21*a43 - a23*a41);
    o.cells[13]  =  a11*(a23*a34 - a24*a33) - a13*(a21*a34 - a24*a31) + a14*(a21*a33 - a23*a31);

    // Third Row of Adjugate (Col 2 of output in Column-Major)
    o.cells[2]  =  a21*(a32*a44 - a34*a42) - a22*(a31*a44 - a34*a41) + a24*(a31*a42 - a32*a41);
    o.cells[6]  = -a11*(a32*a44 - a34*a42) + a12*(a31*a44 - a34*a41) - a14*(a31*a42 - a32*a41);
    o.cells[10] =  a11*(a22*a44 - a24*a42) - a12*(a21*a44 - a24*a41) + a14*(a21*a42 - a22*a41);
    o.cells[14] = -a11*(a22*a34 - a24*a32) + a12*(a21*a34 - a24*a31) - a14*(a21*a32 - a22*a31);

    // Fourth Row of Adjugate (Col 3 of output in Column-Major)
    o.cells[3] = -a21*(a32*a43 - a33*a42) + a22*(a31*a43 - a33*a41) - a23*(a31*a42 - a32*a41);
    o.cells[7] =  a11*(a32*a43 - a33*a42) - a12*(a31*a43 - a33*a41) + a13*(a31*a42 - a32*a41);
    o.cells[11] = -a11*(a22*a43 - a23*a42) + a12*(a21*a43 - a23*a41) - a13*(a21*a42 - a22*a41);
    o.cells[15] =  a11*(a22*a33 - a23*a32) - a12*(a21*a33 - a23*a31) + a13*(a21*a32 - a22*a31);

    return o;
}

float Matrix4::determinant() const {
  // Input Column 0 (indices 0, 1, 2, 3)
  float a11 = cells[0], a21 = cells[1], a31 = cells[2], a41 = cells[3];
  // Input Column 1 (indices 4, 5, 6, 7)
  float a12 = cells[4], a22 = cells[5], a32 = cells[6], a42 = cells[7];
  // Input Column 2 (indices 8, 9, 10, 11)
  float a13 = cells[8], a23 = cells[9], a33 = cells[10], a43 = cells[11];
  // Input Column 3 (indices 12, 13, 14, 15)
  float a14 = cells[12], a24 = cells[13], a34 = cells[14], a44 = cells[15];

  // Compute cofactors for Column 0 only
  float c11 =  (a22*(a33*a44 - a34*a43) - a23*(a32*a44 - a34*a42) + a24*(a32*a43 - a33*a42));
  float c21 = -(a12*(a33*a44 - a34*a43) - a13*(a32*a44 - a34*a42) + a14*(a32*a43 - a33*a42));
  float c31 =  (a12*(a23*a44 - a24*a43) - a13*(a22*a44 - a24*a42) + a14*(a22*a43 - a23*a42));
  float c41 = -(a12*(a23*a34 - a24*a33) - a13*(a22*a34 - a24*a32) + a14*(a22*a33 - a23*a32));

  return a11 * c11 + a21 * c21 + a31 * c31 + a41 * c41;
}

Matrix4 Matrix4::calculateInverse() const {
  float det = determinant();
  float absDet = std::abs(det);
  assert(absDet > 1e-8f);
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

  float m11 = n.x * n.x * oneMinusCos + cosT;
  float m12 = n.x * n.y * oneMinusCos + n.z * sinT;
  float m13 = n.x * n.z * oneMinusCos - n.y * sinT;
  float m14 = 0.0f;
  
  float m21 = n.x * n.y * oneMinusCos - n.z * sinT;
  float m22 = n.y * n.y * oneMinusCos + cosT;
  float m23 = n.y * n.z * oneMinusCos + n.x * sinT;
  float m24 = 0.0f;

  float m31 = n.x * n.z * oneMinusCos + n.y * sinT;
  float m32 = n.y * n.z * oneMinusCos - n.x * sinT;
  float m33 = n.z * n.z *oneMinusCos + cosT;
  float m34 = 0.0f;

  float m41 = 0.0f;
  float m42 = 0.0f;
  float m43 = 0.0f;
  float m44 = 1.0f;
  
  // Column Major Order
  Matrix4 rotationMatrix({
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44
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

  // row 0 (x horizontal FOV)
  // row 1 (y vertical FOV)
  // row 2 (used for z-depth and clipping tests)
  // row 3 (used to preserve original depth information for z)
  return Matrix4({
    horizFov,   0.0f,     0.0f,       0.0f, 
    0.0f,       vertFov,  0.0f,       0.0f,
    0.0f,       0.0f,     zDepth1,    -1.0f,
    0.0f,       0.0f,     zDepth2,    0.0f
  });
}

// Column Major order
Matrix4 MathUtils::lookAt(const Vector3& srcPosition, const Vector3& targetPosition, const Vector3& up) {
  Vector3 forwardBasis(targetPosition - srcPosition);
  forwardBasis.normalize();

  Vector3 upCopy(up);
  // handle gimbal lock case (if foward vector points global (0,1,0) y up change it to be in the global z axis instead)
  if (fabs(forwardBasis.x) < 0.001f && fabs(forwardBasis.z) < 0.001f) {
    if (forwardBasis.y > 0) {
      upCopy = Vector3(0, 0, -1);
    } else {
      upCopy = Vector3(0, 0, 1);
    }
  }

  Vector3 rightBasis(forwardBasis.cross(upCopy));
  rightBasis.normalize();

  Vector3 upBasis(rightBasis.cross(forwardBasis));
  upBasis.normalize();

  float translationX = rightBasis.dot(srcPosition) * -1.0f;
  float translationY = upBasis.dot(srcPosition) * -1.0f;
  float translationZ = forwardBasis.dot(srcPosition);

  return Matrix4({
    rightBasis.x,     rightBasis.y,     rightBasis.z,     0.0f,
    upBasis.x,        upBasis.y,        upBasis.z,        0.0f,
    forwardBasis.x,   forwardBasis.y,   forwardBasis.z,  0.0f,
    translationX,     translationY,     translationZ,     1.0f
  });
}

// TODO: fix this computation!
// TODO: read up on glReadPixels as its primary use case is to allow me to select objects on screen where the gpu will at the depth buffer information at me at that point...
Vector3 MathUtils::screenSpaceToViewSpace(float x, float y, float width, float height, const Matrix4& projectionMatrix) {
  Vector3 ndcCoordinates = MathUtils::convertFromScreenSpaceToNDC(x, y, width, height);

  // In NDC, the values range between the following axes:
  // -1 <= x <= +1
  // -1 <= y <= +1
  // -1 <= z <= +1
  // near clipping plane is mapped to -1
  // far clipping plane is mapped to +1
  ndcCoordinates.z = -1.0f;

  Matrix4 inverse = projectionMatrix.calculateInverse();
  std::vector<float> viewSpaceRaw(inverse.matrixVertMult(ndcCoordinates));

  assert(viewSpaceRaw.size() == 4);
  // w component must not be zero!
  assert(viewSpaceRaw[3] != 0.0f);

  // Perspective Divide by w to ensure the coordinates in view space are scaled correctly
  float w = viewSpaceRaw[3];
  Vector3 viewSpaceCoords(viewSpaceRaw[0] / w, viewSpaceRaw[1] / w, viewSpaceRaw[2] / w);

  return viewSpaceCoords;
}

Vector3 MathUtils::screenSpaceToWorldSpace(float x, float y, float z, float width, float height, float near, float far, const Matrix4& projectionMatrix, const Matrix4& viewMatrix, bool isPerspective) {
  assert(near > 0.0f);
  assert(far > near);
  assert(z != 0.0f);

  Vector3 ndcCoordinates(MathUtils::convertFromScreenSpaceToNDC(x, y, width, height));
  Vector3 worldSpaceCoordinates;
  std::vector<float> world4DCoordinates;

  // Perspective Conversion
  float wclip = 1.0f;
  float zclip = 1.0f;
  if (isPerspective) {
    // convert z eye distance to z ndc coordinates
    // The formula comes from the zDepth1 and zDepth2 formulas from the createPerspectiveMatrix (converts view space coordinates to clip coordinates to ndc coordinates)
    zclip = z * ((far+near)/(far-near)) - ((2.0f * far * near) / (far - near));
    wclip = z;
    ndcCoordinates.z = zclip / wclip;
  } else {
    // Orthographic Conversion
    zclip = (-1.0f * z * ((2.0f) / (far - near))) + (-1.0f * (far + near) / (far - near));
    wclip = 1.0f;
    ndcCoordinates.z = zclip / wclip;
  }

  Vector3 clipCoords(
    ndcCoordinates.x * wclip,
    ndcCoordinates.y * wclip,
    zclip
  );

  Matrix4 projViewMat = projectionMatrix * viewMatrix;
  Matrix4 invMat = projViewMat.calculateInverse();

  world4DCoordinates = invMat.matrixVertMult(clipCoords, wclip);

  assert(world4DCoordinates.size() == 4);
  assert(world4DCoordinates[3] != 0.0f);

  float w = world4DCoordinates[3];
  worldSpaceCoordinates = Vector3(world4DCoordinates[0] / w, world4DCoordinates[1] / w, world4DCoordinates[2] / w);

  return worldSpaceCoordinates;
}