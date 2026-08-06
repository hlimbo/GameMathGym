#ifndef MATRIX4_H
#define MATRIX4_H

#include <assert.h>
#include <vector>
#include <iostream>
#include <initializer_list>

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
    float cells[MAT4_SIZE] {};

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

    constexpr Matrix4(std::initializer_list<float> list) {
      assert(MAT4_SIZE == list.size());
      int i = 0;
      for (float val : list) {
        cells[i] = val;
        ++i;
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
    // return the 4th dimension for inverse matrix calculations
    std::vector<float> matrixVertMult(const Vector3& rhs) const;
    std::vector<float> matrixVertMult(const Vector3& rhs, float w) const;
    Vector3 operator*(const Vector3& rhs) const;

    // readonly [] subscript operator -- returns a copy of the float held in the matrix
    float operator[](int i) const;

    void transpose();

    Matrix4 operator*(const float scalar) const;
    friend Matrix4 operator*(const float scalar, const Matrix4& rhs);
    Matrix4 operator*=(const float scalar);

    Matrix4 operator+(const Matrix4& rhs) const;

    float determinant() const;
    Matrix4 adjugate() const;
    Matrix4 calculateInverse() const;
  };

  Matrix4 operator*(const float scalar, const Matrix4& rhs);

  Matrix4 makeTranslationMatrix(const Vector3& offset);
  Matrix4 makeScaleMatrix(float scale);
  Matrix4 makeScaleMatrix(const Vector3& scalars);

  // creates a rotation matrix to multiply other vectors with to rotate the vectors
  Matrix4 rotate(const Matrix4& m, const float anglesRadians, const Vector3& normalizedAxis);
  // returns a vector3 that is rotated by m rotationMatrix
  Vector3 rotate(const Matrix4& m, const Vector3& v);

  // Will be in column major form as OpenGL uses column major form for their computations
  Matrix4 createOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);
  
  /*
    - supports assymetrical perspective cameras
  */
  Matrix4 createPerspectiveMatrix(float l, float r, float b, float t, float n, float f);
  /*
    - supports symmetrical perspective cameras
    Parameters:
    - aspect - aspect ratio = width / height -> used to ensure proportion along x-axis looks visually correct for rectangular screens
    - fov - vertical field of view - takes in range between [0, 180) degrees. internally converts degrees to radians. Higher values means you can see a wider view whereas lower values means you can see a narrower view.
    - n - near clipping plane distance
    - f - far clipping plane distance
  */
  Matrix4 createPerspectiveMatrix(float aspect, float fov, float n, float f);

  Matrix4 lookAt(const Vector3& srcPosition, const Vector3& targetPosition, const Vector3& up);

  Vector3 screenSpaceToViewSpace(float x, float y, float width, float height, const Matrix4& projectionMatrix);
  /*
    Screen Space Coordinates ranges:
      - x-axis: 0 to width
      - y-axis: 0 to height
      - In SDL3, screen coordinates on y-axis are inverted. Top left corner is (0,0) and bottom right corner is (width, height)
  
    Parameters:
    - x - x-axis coordinates in screen space (measured in pixels)
    - y - y-axis coordinates in screen space (measured in pixels)
    - z - z represents the distance from the camera in world units (eye distance)
    - width - screen width in pixels
    - height - screen height in pixels
    - near - near clipping plane distance (must be > 0 and < far)
    - far - far clipping plane distance (must be > 0 and > near)
    - projectionMatrix - can either be the perspective matrix or orthographic matrix
    - viewMatrix - camera's view matrix
    - isPerspective - if true, it is using a perspective projection matrix; false it defaults to an orthographic projection matrix

    Converts screen space to world space coordinates
  */
  Vector3 screenSpaceToWorldSpace(float x, float y, float z, float width, float height, float near, float far, const Matrix4& projectionMatrix, const Matrix4& viewMatrix, bool isPerspective);

  /*
    Parameters:
    - x - x-axis coordinates in screen space (measured in pixels)
    - y - y-axis coordinates in screen space (measured in pixels)
    - width - screen width in pixels
    - height - screen height in pixels
    - projectionMatrix - Matrix4
    - viewMatrix - Matrix4

    Returns a unit vector pointing towards the game world from screen space coordinates
  */
  Vector3 screenSpaceToWorldDirection(float x, float y, float width, float height, const Matrix4& projectionMatrix, const Matrix4& viewMatrix);

  constexpr Matrix4 MAT4_IDENTITY({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  });

  constexpr float PI = 3.14159265358979323846f;
}


#endif // MATRIX4_H