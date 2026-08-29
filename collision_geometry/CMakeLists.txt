set(AABB_TESTS "AABBUnitTests")

add_executable(${AABB_TESTS})
target_sources(
  ${AABB_TESTS}
  PRIVATE
    aabb_tests.cpp
    aabb.cpp
    "../math_utils/vector3.cpp"
)

target_include_directories(
  ${AABB_TESTS}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(
  ${AABB_TESTS}
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)

add_test(AABBTests ${AABB_TESTS})

set(SAT_TESTS "SATUnitTests")

add_executable(${SAT_TESTS})
target_sources(
  ${SAT_TESTS}
  PRIVATE
    sat_tests.cpp
    sat.cpp
    "../math_utils/vector3.cpp"
)

target_include_directories(
  ${SAT_TESTS}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(
  ${SAT_TESTS}
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)

add_test(SATTests ${SAT_TESTS})