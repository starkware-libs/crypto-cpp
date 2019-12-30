#include "starkware/utils/prng.h"

#include <algorithm>

#include "gtest/gtest.h"

#include "starkware/utils/math.h"

namespace starkware {
namespace {

TEST(Prng, PartialRange) {
  Prng prng;
  uint64_t min_num = std::numeric_limits<uint64_t>::max();
  uint64_t max_num = 0;
  for (size_t i = 0; i < 1000; i++) {
    const uint64_t rnd = prng.RandomUint64(900, 910);
    min_num = std::min(min_num, rnd);
    max_num = std::max(max_num, rnd);
  }
  // These should fail with negligible probability (~2^(-137)).
  EXPECT_EQ(min_num, 900);
  EXPECT_EQ(max_num, 910);
}

TEST(Prng, FullRange) {
  Prng prng;
  uint64_t max_num = 0;
  for (size_t i = 0; i < 100; i++) {
    const uint64_t rnd = prng.RandomUint64();
    max_num = std::max(max_num, rnd);
  }
  // This should fail with negligible probability (2^(-100)).
  EXPECT_GT(max_num, Pow2(63));
}

}  // namespace
}  // namespace starkware
