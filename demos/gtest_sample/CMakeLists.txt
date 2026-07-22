set(PROGRAM_NAME "GTestSample")
add_executable(${PROGRAM_NAME})
target_sources(
  ${PROGRAM_NAME}
  PRIVATE
    gtest_sample.cpp
)
target_link_libraries(
  ${PROGRAM_NAME}
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)

# Registers a suite of tests to run
add_test(AllTestsInGTestSample ${PROGRAM_NAME})

# Discovers and finds all tests suites in the target GTestSample
# include(GoogleTest)
# gtest_discover_tests("GTestSample")