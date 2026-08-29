/*
  Separating Axis Theorem
*/
#include "sat.h"
#include "math_utils/vector3.h"

std::vector<MathUtils::Vector3> CollisionGeometry::FindAllNormalAxes(const std::vector<MathUtils::Vector3> &vertices)
{
  return std::vector<MathUtils::Vector3>();
}

bool CollisionGeometry::CheckShapesOverlapOneAxisSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2, const MathUtils::Vector3 &axis)
{
  return false;
}

bool CollisionGeometry::CheckShapesOverlapAllAxesSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2, const std::vector<MathUtils::Vector3> &axes)
{
  return false;
}

bool CollisionGeometry::CheckShapesOverlapSAT(const std::vector<MathUtils::Vector3> &v1, const std::vector<MathUtils::Vector3> &v2)
{
  return false;
}
