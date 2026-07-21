#include <gtest/gtest.h>
#include <iostream>

TEST(AllTestsInGTestSample, BasicAssertions) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);
}

TEST(AllTestsInGTestSample, NextTests) {
  EXPECT_STREQ("bye", "bye");
  EXPECT_GT(5, 2);
}

int main(int argc, char* argv[]) {
  std::cout << "Running The Sample GTests" << std::endl;

  // initialize google test with command line arguments
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}