/*
  Separating Axis Theorem
*/
#include "sat.h"
#include "math_utils/vector3.h"

#include <algorithm>
#include <iostream>
#include <limits>

std::vector<MathUtils::Vector3> CollisionGeometry::CalculateFaceNormalAxes(const std::vector<MathUtils::Vector3> &vertices, const std::vector<uint32_t>& indices)
{
  std::vector<MathUtils::Vector3> normals;

  // this generates duplicate normals (e.g. for a cube it will generate a normal vector, one for each triangle that faces the same direction)
  for (uint32_t i = 2; i < indices.size(); i += 3) {
    uint32_t i0 = indices[i - 2];
    uint32_t i1 = indices[i - 1];
    uint32_t i2 = indices[i];
    MathUtils::Vector3 v0(vertices[i0]);
    MathUtils::Vector3 v1(vertices[i1]);
    MathUtils::Vector3 v2(vertices[i2]);

    MathUtils::Vector3 e1(v1 - v0);
    MathUtils::Vector3 e2(v2 - v0);
    MathUtils::Vector3 normal = e1.cross(e2);

    if (normal.sqrMagnitude() == 0.0f) {
      continue;
    }

    normal.normalize();
    normals.push_back(normal);
  }

  return normals;
}


// It will also include the diagonals of the shape as edges where I assume the 3d shape is convex and triangulated
std::vector<MathUtils::Vector3> CollisionGeometry::CalculateEdgeDirectionAxes(const std::vector<MathUtils::Vector3> &vertices, const std::vector<uint32_t> &indices)
{
  std::vector<MathUtils::Vector3> edgeDirectionAxes;

  for (uint32_t i = 1, j = i - 1; i < indices.size(); ++i) {
    MathUtils::Vector3 edge(vertices[i] - vertices[j]);
    
    // skip edges that can't be normalized into unit vectors
    if (edge.sqrMagnitude() == 0.0f) {
      continue;
    }
    
    edge.normalize();
    edgeDirectionAxes.push_back(edge);
  }

  return edgeDirectionAxes;
}

bool CollisionGeometry::CheckShapesOverlapOneAxisSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2, const MathUtils::Vector3 &axis)
{
  // bounds for v1 shape
  float minBound1 = std::numeric_limits<float>::max(), maxBound1 = std::numeric_limits<float>::min();
  // bounds for v2 shape
  float minBound2 = std::numeric_limits<float>::max(), maxBound2 = std::numeric_limits<float>::min();

  for (const MathUtils::Vector3& v : v1) {
    // vector projection formula = ((v * axis) / pow(magnitude(axis), 2)) * axis
    // since we only want the scalar value and axis is a unit vector, the projection formula reduces down to:
    // projection scalar = (v * axis)
    float projection = v.dot(axis);
    minBound1 = std::min(projection, minBound1);
    maxBound1 = std::max(projection, maxBound1);
  }

  for (const MathUtils::Vector3& v : v2) {
    float projection = v.dot(axis);
    minBound2 = std::min(projection, minBound2);
    maxBound2 = std::max(projection, maxBound2);
  }

  bool hasOverlap = minBound2 <= maxBound1 || minBound1 <= maxBound2;
  return hasOverlap;
}

bool CollisionGeometry::CheckShapesOverlapAllAxesSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2, const std::vector<MathUtils::Vector3>& axes)
{
  if (v1.size() == 0 || v2.size() == 0 || axes.size() == 0) {
    return false;
  }

  for (const MathUtils::Vector3& axis : axes) {
    // if can find 1 axis where the 2 convex shapes do not overlap, then by SAT, the 2 shapes do not overlap
    bool hasNoOverlap = !CheckShapesOverlapOneAxisSAT(v1, v2, axis); 
    if (hasNoOverlap) {
      return false;
    }
  }

  // returns true if on all axes the 2 convex shapes overlap
  return true;
}

bool CollisionGeometry::CheckShapesOverlapSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2, const std::vector<uint32_t>& i1, const std::vector<uint32_t>& i2)
{
  // Obtain all axes to be used in SAT collision test -- to be better optimized, I could precompute the face normals and store them in a separate data structure
  std::vector<MathUtils::Vector3> faceNormals1 = CalculateFaceNormalAxes(v1, i1);
  std::vector<MathUtils::Vector3> faceNormals2 = CalculateFaceNormalAxes(v2, i2);
  std::vector<MathUtils::Vector3> edgeVectors1 = CalculateEdgeDirectionAxes(v1, i1);
  std::vector<MathUtils::Vector3> edgeVectors2 = CalculateEdgeDirectionAxes(v2, i2);

  std::vector<MathUtils::Vector3> axes;
  
  for (const MathUtils::Vector3& axis : faceNormals1) {
    bool isNotFound = std::find(axes.begin(), axes.end(), axis) == axes.end();
    if (isNotFound) {
      axes.push_back(axis);
    }
  }

  for (const MathUtils::Vector3& axis : faceNormals2) {
    bool isNotFound = std::find(axes.begin(), axes.end(), axis) == axes.end();
    if (isNotFound) {
      axes.push_back(axis);
    }
  }

  // Need to get the cross product of edgeVectors1[i] x edgeVectors2[j] to obtain potential edge axis separation
  for (const MathUtils::Vector3& e1 : edgeVectors1) {
    for (const MathUtils::Vector3& e2 : edgeVectors2) {
      MathUtils::Vector3 pendingAxis = e1.cross(e2);
      // skip cross products whose magnitude is 0 as it can't be used as a valid axis for SAT
      if (pendingAxis.magnitude() == 0.0f) {
        continue;
      }

      axes.push_back(pendingAxis);
    }
  }

  return CheckShapesOverlapAllAxesSAT(v1, v2, axes);
}
