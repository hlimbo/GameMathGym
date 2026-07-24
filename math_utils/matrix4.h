#ifndef MATRIX4_H
#define MATRIX4_H

#include <assert.h>
#include <vector>
#include <iostream>

namespace MathUtils {
  // Forward Declarations
  struct Vector3;

  constexpr int MAT4_DIM = 4;
  constexpr int MAT4_SIZE = MAT4_DIM * MAT4_DIM;

  /*
      In OpenGL, values are stored in column major order when processing matrices.
      The Matrix4 class will store values in Col Major.

      Col Major
      0   4   8   12
      1   5   9   13
      2   6   10  14
      3   7   11  15

      2D to 1D 
      index = col_index * (rowsPerCol) + row_index
  */
  struct Matrix4 {
    float cells[MAT4_SIZE];

    Matrix4(): cells{
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f
     } {}

     Matrix4(float c[MAT4_SIZE]): cells{
      c[0], c[1], c[2], c[3],
      c[4], c[5], c[6], c[7],
      c[8], c[9], c[10],c[11],
      c[12],c[13],c[14],c[15]
     } {}

    Matrix4(const std::vector<float>& d) {
      assert(d.size() == MAT4_SIZE);
      for (int i = 0;i < MAT4_SIZE; ++i) {
        cells[i] = d[i];
      }
    }

    Matrix4(const Matrix4& other): cells {
      other.cells[0], other.cells[1], other.cells[2], other.cells[3],
      other.cells[4], other.cells[5], other.cells[6], other.cells[7],
      other.cells[8], other.cells[9], other.cells[10], other.cells[11],
      other.cells[12], other.cells[13], other.cells[14], other.cells[15],
    } {}

    Matrix4 operator*(const Matrix4& rhs) const;
    Matrix4 operator*=(const Matrix4& rhs);

    // will internally convert a Vector3 to have an extra dimension
    Vector3 operator*(const Vector3& rhs) const;

    // readonly [] subscript operator -- returns a copy of the float held in the matrix
    float operator[](int i) const;

    void transpose();

    Matrix4 operator*(const float scalar) const;
    friend Matrix4 operator*(const float scalar, const Matrix4& rhs);
    Matrix4 operator*=(const float scalar);

    Matrix4 operator+(const Matrix4& rhs) const;
  };

  Matrix4 operator*(const float scalar, const Matrix4& rhs);

  Matrix4 makeTranslationMatrix(const Vector3& offset);

  // creates a rotation matrix to multiply other vectors with to rotate the vectors
  Matrix4 rotate(const Matrix4& m, const float degreesRadians, const Vector3& normalizedAxis);
  // returns a vector3 that is rotated by m rotationMatrix
  Vector3 rotate(const Matrix4& m, const Vector3& v);

  const Matrix4 MAT4_IDENTITY({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  });

  constexpr float PI = 3.14159265358979323846f;
}


#endif // MATRIX4_H