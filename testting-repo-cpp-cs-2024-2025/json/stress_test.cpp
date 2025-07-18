#include <gtest/gtest.h>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <cstdlib>
#include <format>
#include <cctype>
#include <algorithm>


#include "stress_utils.hpp"
#include "utils.hpp"
#include "json.hpp"

TEST(Stress, Large) {
  auto value = Level<2>{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/StressLarge.json"));
}

TEST(Stress, Long) {
  auto value = ExtraLong<512ul>{};
  auto converted = ConvertToJson(value);
  auto answer = std::string();
  std::ifstream f("testdata/StressExtraLong.json");
  f >> answer;
  Trim(converted);
  Trim(answer);
  EXPECT_EQ(converted, answer);
}

TEST(Stress, Exp) {
  auto value = ExpWide<20>{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/StressExpWide.json"));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
