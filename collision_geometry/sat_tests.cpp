#include <algorithm>
#include <gtest/gtest.h>


#include "sat.h"
#include "math_utils/vector3.h"

using namespace CollisionGeometry;
using namespace MathUtils;

TEST(SAT_TESTS, SAT_Sanity_Test)
{
  std::vector<Vector3> axes = CollisionGeometry::CalculateFaceNormalAxes(std::vector<Vector3>(), std::vector<uint32_t>());
  bool hasOverlap = CollisionGeometry::CheckShapesOverlapOneAxisSAT(std::vector<Vector3>(), std::vector<Vector3>(), Vector3());
  bool hasAllOverlaps = CollisionGeometry::CheckShapesOverlapAllAxesSAT(std::vector<Vector3>(), std::vector<Vector3>(), std::vector<Vector3>());
  bool doesOverlap = CollisionGeometry::CheckShapesOverlapSAT(std::vector<Vector3>(), std::vector<Vector3>(), std::vector<uint32_t>(), std::vector<uint32_t>());

  EXPECT_EQ(0, axes.size());
  EXPECT_FALSE(hasOverlap);
  EXPECT_FALSE(hasAllOverlaps);
  EXPECT_FALSE(doesOverlap);
}

TEST(SAT_TESTS, SAT_Verify_Cube_Face_Normals_Calculation)
{
  /*
    Cube Data Vertices added in here rather than using Shapes::Cube
    because the constructor of Cube uses OpenGL related functions to initialize its data
    onto the GPU
  */
  std::vector<Vector3> cubeVertices {
    // front face
    Vector3 { -0.5f, 0.5f, 0.5f },
    Vector3 { 0.5f, 0.5f, 0.5f },
    Vector3 { -0.5f, -0.5f, 0.5f },
    Vector3 { 0.5f, -0.5f, 0.5f },

    // back face
    Vector3 { -0.5f, 0.5f, -0.5f },
    Vector3 { 0.5f, 0.5f, -0.5f },
    Vector3 { -0.5f, -0.5f, -0.5f },
    Vector3 { 0.5f, -0.5f, -0.5f },

    //left face
    Vector3 { -0.5f, 0.5f, 0.5f },    
    Vector3 { -0.5f, 0.5f, -0.5f },    
    Vector3 { -0.5f, -0.5f, 0.5f },    
    Vector3 { -0.5f, -0.5f, -0.5f },
    
    // right face
    Vector3 { 0.5f, 0.5f, 0.5f },
    Vector3 { 0.5f, 0.5f, -0.5f },
    Vector3 { 0.5f, -0.5f, 0.5f },
    Vector3 { 0.5f, -0.5f, -0.5f },

    // top face
    Vector3 { -0.5f, 0.5f, 0.5f },
    Vector3 { 0.5f, 0.5f, 0.5f },
    Vector3 { -0.5f, 0.5f, -0.5f },
    Vector3 { 0.5f, 0.5f, -0.5f },

    // bottom face
    Vector3 { -0.5f, -0.5f, 0.5f },
    Vector3 { 0.5f, -0.5f, 0.5f },
    Vector3 { -0.5f, -0.5f, -0.5f },
    Vector3 { 0.5f, -0.5f, -0.5f },
  };

  std::vector<uint32_t> cubeIndices {
    // front face
    1, 0, 2,
    1, 2, 3,

    // Back Face
    4, 5, 6,
    6, 5, 7,

    // Left Face
    11, 8, 9,
    10, 8, 11,

    // Right Face
    12, 14, 13,
    15, 13, 14,

    // Top Face
    19, 18, 16,
    17, 19, 16,

    // Bot Face
    20, 22, 23,
    20, 23, 21
  };


  std::vector<Vector3> actualFaceNormals = CollisionGeometry::CalculateFaceNormalAxes(cubeVertices, cubeIndices);

  std::vector<Vector3> expectedFaceNormals  {
    Vector3 { 0.0f, 0.0f, 1.0f },
    Vector3 { 0.0f, 0.0f, -1.0f },
    Vector3 { -1.0f, 0.0f, 0.0f },
    Vector3 { 1.0f, 0.0f, 0.0f },
    Vector3 { 0.0f, 1.0f, 0.0f },
    Vector3 { 0.0f, -1.0f, 0.0f }
  };
  

  for (const Vector3& expectedFaceNormal : expectedFaceNormals ) {
    bool isFaceNormalFound = std::find(actualFaceNormals.begin(), actualFaceNormals.end(), expectedFaceNormal) != actualFaceNormals.end();
    EXPECT_TRUE(isFaceNormalFound) << "Assertion Failed with expectedFaceNormal " << expectedFaceNormal << " not found in actualFaceNormals list\n";
  }
 

}