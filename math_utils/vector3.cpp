#include "vector3.h"
using namespace MathUtils;

#include <assert.h>
#include <cmath>

Vector3 Vector3::operator+(const Vector3& rhs) const {
  return Vector3(x+rhs.x,y+rhs.y,z+rhs.z);
}

Vector3 Vector3::operator+=(const Vector3& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
  return Vector3(x-rhs.x,y-rhs.y,z-rhs.z);
}

Vector3 Vector3::operator-=(const Vector3& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3 Vector3::operator*(const float scalar) const{
  return Vector3(scalar * x, scalar * y, scalar * z);
}

Vector3 MathUtils::operator*(float scalar, const Vector3& rhs) {
  return rhs * scalar;
}

Vector3 Vector3::operator*=(const float scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

Vector3 Vector3::operator/(const float scalar) const {
  assert(scalar != 0.0f);
  return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator/=(const float scalar) {
  assert(scalar != 0.0f);
  x /= scalar;
  y /= scalar;
  z /= scalar;
  return *this;
}

float Vector3::dot(const Vector3& rhs) const {
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::cross(const Vector3& rhs) const {
  return Vector3(
    y * rhs.z - z * rhs.y,
    z * rhs.x - x * rhs.z,
    x * rhs.y - y * rhs.x
  );
}

float Vector3::sqrMagnitude() const {
  return x * x + y * y + z * z;
}

float Vector3::magnitude() const {
  return sqrtf(x * x + y * y + z * z);
}

void Vector3::normalize() {
  float divisor = this->magnitude();
  *this /= divisor;
}