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