#include <cassert>
#include "utils.hpp"
#include "stress_utils.hpp"
#include "json.hpp"

void ExtraStressTest() {
  auto value = Level1{};
  assert(CompareWithFile(value, "testdata/ExtraStressVeryLarge.json"));
} 


int main() {
  ExtraStressTest();
}
