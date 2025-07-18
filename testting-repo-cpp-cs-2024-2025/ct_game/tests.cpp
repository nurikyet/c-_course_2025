#include <gtest/gtest.h>

#include "ct_game.hpp"


#ifndef SOLVE
#define SOLVE(...) SolutionLength({__VA_ARGS__})
#endif

TEST(Eights, Dummy) {
  {
    constexpr auto kAnswer = 0;
    constexpr auto kResult = SOLVE(
      1, 2, 3,
      4, 5, 6,
      7, 8, 0
    );
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

TEST(Eights, Small) {
  {
    constexpr auto kAnswer = 8;
    constexpr auto kResult = SOLVE(
      0, 1, 6,
      4, 3, 2,
      7, 5, 8
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 8;
    constexpr auto kResult = SOLVE(
      4, 1, 2,
      7, 6, 3,
      0, 5, 8
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 9;
    constexpr auto kResult = SOLVE(
      4, 1, 2,
      5, 8, 0,
      7, 6, 3
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

TEST(Eights, Medium) {
  {
    constexpr auto kAnswer = 11;
    constexpr auto kResult = SOLVE(
      1, 2, 4,
      0, 5, 3,
      7, 8, 6
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 12;
    constexpr auto kResult = SOLVE(
      2, 4, 3,
      7, 5, 6,
      8, 1, 0
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 16;
    constexpr auto kResult = SOLVE(
      4, 2, 3,
      7, 0, 5,
      8, 6, 1
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 18;
    constexpr auto kResult = SOLVE(
      4, 2, 0,
      6, 3, 8,
      1, 7, 5
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

TEST(Eights, Large) {
  {
    constexpr auto kAnswer = 28;
    constexpr auto kResult = SOLVE(
      3, 2, 7,
      5, 8, 4,
      6, 1, 0
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = 30;
    constexpr auto kResult = SOLVE(
      1, 4, 7,
      2, 0, 8,
      3, 6, 5
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

TEST(Eights, Impossible) {
  {
    constexpr auto kAnswer = -1;
    constexpr auto kResult = SOLVE(
      7, 5, 6,
      1, 0, 3,
      8, 2, 4
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = -1;
    constexpr auto kResult = SOLVE(
        3, 7, 5,
        6, 2, 0,
        1, 8, 4
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

TEST(Eights, Invalid) {
  {
    constexpr auto kAnswer = -2;
    constexpr auto kResult = SOLVE(
      1, 2, 3,
      4, 5, 6,
      7, 7, 0
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = -2;
    constexpr auto kResult = SOLVE(
      1, 2, 3,
      4, 5, 6,
      7, 0, 9
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }

  {
    constexpr auto kAnswer = -2;
    constexpr auto kResult = SOLVE(
      0, 0, 0,
      0, 0, 0,
      0, 0, 0
    ); 
    static_assert(kResult == kAnswer);
    EXPECT_EQ(kResult, kAnswer);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
