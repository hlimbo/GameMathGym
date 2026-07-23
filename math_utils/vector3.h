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
  };

  Vector3 operator*(float scalar, const Vector3& rhs);
}

#endif // VECTOR3_H