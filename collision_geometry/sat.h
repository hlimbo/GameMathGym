#ifndef SAT_H
#define SAT_H

#include <vector>

/*
  Separating Axis Theorem
*/
namespace MathUtils {
  struct Vector3;
}

namespace CollisionGeometry {
  /*
    returns list of normal unit vectors
  */
  std::vector<MathUtils::Vector3> FindAllNormalAxes(const std::vector<MathUtils::Vector3>& vertices);
  
  /*
    returns true if 2 shapes overlap using 1 common axis that is a unit vector, false otherwise
  */
 bool CheckShapesOverlapOneAxisSAT(const std::vector<MathUtils::Vector3>& v1, const std::vector<MathUtils::Vector3>& v2, const MathUtils::Vector3& axis);
  
  /*
    returns true if 2 shapes overlap with all possible axes given, false otherwise
  */
  bool CheckShapesOverlapAllAxesSAT(const std::vector<MathUtils::Vector3>& v1, const std::vector<MathUtils::Vector3>& v2, const std::vector<MathUtils::Vector3>& axes);

  bool CheckShapesOverlapSAT(const std::vector<MathUtils::Vector3>& v1, const std::vector<MathUtils::Vector3>& v2);
}


#endif /* SAT_H */
