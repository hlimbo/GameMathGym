set(VEC3_TESTS "Vector3UnitTests")

add_executable(${VEC3_TESTS})
target_sources(
  ${VEC3_TESTS}
  PRIVATE
    vector3_tests.cpp
    vector3.cpp
)

target_link_libraries(
  ${VEC3_TESTS}
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)

add_test(Vector3Tests ${VEC3_TESTS})

set(MAT4_TESTS "Matrix4UnitTests")

add_executable(${MAT4_TESTS})
target_sources(
  ${MAT4_TESTS}
  PRIVATE
    matrix4_tests.cpp
    matrix4.cpp
)

target_link_libraries(
  ${MAT4_TESTS}
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)

add_test(Matrix4Tests ${MAT4_TESTS})