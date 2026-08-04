#ifndef VECTOR3_H
#define VECTOR3_H

namespace MathUtils {
  struct Vector3 {
    float x, y, z;

    Vector3(): x(0.0f), y(0.0f), z(0.0f) {}
    explicit Vector3(float _s): x(_s), y(_s), z(_s) {}
    Vector3(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
    Vector3(const Vector3& other): x(other.x), y(other.y), z(other.z) {}

    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3 operator+=(const Vector3& rhs);
    Vector3 operator-=(const Vector3& rhs);

    Vector3 operator*(const float scalar) const;
    Vector3 operator/(const float scalar) const;
    Vector3 operator*=(const float scalar);
    Vector3 operator/=(const float scalar);

    // support for multiplying on left side
    friend Vector3 operator*(float scalar, const Vector3& rhs);

    float dot(const Vector3& rhs) const;
    Vector3 cross(const Vector3& rhs) const;
    // vector length
    float magnitude() const;
    // vector square length
    float sqrMagnitude() const;
    void normalize();
    Vector3 normalized() const;
  };

  Vector3 operator*(float scalar, const Vector3& rhs);

  /*
    - converts SDL3 screen space coordinates that range for the following dimensions:
      - x => [0, SCREEN_WIDTH]
      - y => [0, SCREEN_HEIGHT]

    TO
      Normalized Device Coordinates NDC
      - x => [-1, 1]
      - y => [-1, 1]

    Note: since the origin of screen space coordinates starts on the top left corner of the application window
    and y grows downwards, this code will invert y to ensure y remaps to [-1,1] as Normalized Device Coordinates
    origin is the center of the screen.

    RETURNS: Vector3 containing the NDC coordinates for x and y axis. The z axis will be 0 as its not computed in this function.
  */
  Vector3 convertFromScreenSpaceToNDC(float screenX, float screenY, float screenWidth, float screenHeight);
}

#endif // VECTOR3_H