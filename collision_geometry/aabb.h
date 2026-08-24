#ifndef AABB_H
#define AABB_H

/* Forward Declarations */
namespace MathUtils {
  struct Vector3;
}

namespace CollisionGeometry {
  struct AABB {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float width, height, depth;

    AABB();
    AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    AABB(const AABB& other);
    AABB(const MathUtils::Vector3& point, const MathUtils::Vector3& dimensions);
  };

  bool IsPointInsideAABB(const MathUtils::Vector3& point, const AABB& aabb);
  bool DoesAABBIntersect(const AABB& aabb1, const AABB& aabb2);
}

#endif /* AABB_H */