#include "aabb.h"
#include "math_utils/vector3.h"

CollisionGeometry::AABB::AABB() :
  minX(0.0f), minY(0.0f), minZ(0.0f),
  maxX(0.0f), maxY(0.0f), maxZ(0.0f),
  width(0.0f), height(0.0f), depth(0.0f)
{}

CollisionGeometry::AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) :
  minX(minX), minY(minY), minZ(minZ),
  maxX(maxX), maxY(maxY), maxZ(maxZ),
  width(maxX - minX), height(maxY - minY), depth(maxZ - minZ)
{}

CollisionGeometry::AABB::AABB(const AABB &other) :
  minX(other.minX), minY(other.minY), minZ(other.minZ),
  maxX(other.maxX), maxY(other.maxY), maxZ(other.maxZ),
  width(other.maxX - other.minX), height(other.maxY - other.minY), depth(other.maxZ - other.minZ)
{}

CollisionGeometry::AABB::AABB(const MathUtils::Vector3 &point, const MathUtils::Vector3 &dimensions) :
  minX(point.x), minY(point.y), minZ(point.z),
  maxX(point.x + dimensions.x), maxY(point.y + dimensions.y), maxZ(point.z + dimensions.z),
  width(dimensions.x), height(dimensions.y), depth(dimensions.z)
{}

bool CollisionGeometry::IsPointInsideAABB(const MathUtils::Vector3 &point, const AABB &aabb)
{
  return
    aabb.minX <= point.x && point.x <= aabb.maxX &&
    aabb.minY <= point.y && point.y <= aabb.maxY &&
    aabb.minZ <= point.z && point.z <= aabb.maxZ;
}

bool CollisionGeometry::DoesAABBIntersect(const AABB &aabb1, const AABB &aabb2)
{
  return 
    aabb1.minX <= aabb2.maxX &&
    aabb1.maxX >= aabb2.minX &&
    aabb1.minY <= aabb2.maxY &&
    aabb1.maxY >= aabb2.minY &&
    aabb1.minZ <= aabb2.maxZ &&
    aabb1.maxZ >= aabb2.minZ;
}
