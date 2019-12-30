#include "starkware/utils/math.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "starkware/utils/prng.h"
#include "starkware/utils/test_utils.h"

namespace starkware {
namespace {

using testing::HasSubstr;

TEST(Math, Pow2) { EXPECT_EQ(32U, Pow2(5)); }
TEST(Math, Pow2_0) { EXPECT_EQ(1U, Pow2(0)); }
TEST(Math, Pow2_63) { EXPECT_EQ(UINT64_C(0x8000000000000000), Pow2(63)); }

TEST(Math, Log2Floor_0) { EXPECT_ASSERT(Log2Floor(0), HasSubstr("log2 of 0 is undefined")); }
TEST(Math, Log2Floor_1) { EXPECT_EQ(Log2Floor(1), 0U); }
TEST(Math, Log2Floor_31) { EXPECT_EQ(Log2Floor(31), 4U); }
TEST(Math, Log2Floor_32) { EXPECT_EQ(Log2Floor(32), 5U); }
TEST(Math, Log2Floor_33) { EXPECT_EQ(Log2Floor(33), 5U); }
TEST(Math, Log2Floor_AllBitsSet) { EXPECT_EQ(Log2Floor(UINT64_C(0xffffffffffffffff)), 63U); }

TEST(Math, GenericPow) {
  Prng prng;
  const uint64_t random_base = prng.RandomUint64(0, 31);
  const size_t exponent = 11;
  std::vector<bool> exp_bits{true, true, false, true};
  // Create pow result.
  uint64_t pow_res = 1;
  for (size_t i = 0; i < exponent; ++i) {
    pow_res *= random_base;
  }

  EXPECT_EQ(
      pow_res,
      GenericPow(random_base, exp_bits, uint64_t(1), [](const uint64_t& multiplier, uint64_t* dst) {
        *dst *= multiplier;
      }));
}

}  // namespace
}  // namespace starkware
