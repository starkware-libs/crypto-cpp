#include "starkware/algebra/big_int.h"

#include <limits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "starkware/utils/test_utils.h"

namespace starkware {
namespace {

using testing::HasSubstr;

TEST(BigInt, Random) {
  Prng prng;
  for (size_t i = 0; i < 100; ++i) {
    BigInt<2> a = BigInt<2>::RandomBigInt(&prng);
    BigInt<2> b = BigInt<2>::RandomBigInt(&prng);
    ASSERT_NE(a, b);
  }
}

TEST(BigInt, Div) {
  BigInt<2> a({0, 1});
  BigInt<2> b({5, 0});

  EXPECT_EQ(a.Div(b), std::make_pair(BigInt<2>({0x3333333333333333L, 0}), BigInt<2>({1, 0})));
}

TEST(BigInt, DivByZero) {
  BigInt<2> a({0, 1});

  EXPECT_ASSERT(a.Div(BigInt<2>(0)), testing::HasSubstr("must not be zero"));
}

TEST(BigInt, DivRandom) {
  Prng prng;
  const BigInt<2> a = BigInt<2>::RandomBigInt(&prng);
  const BigInt<2> b = BigInt<2>::RandomBigInt(&prng);
  const auto& [q, r] = a.Div(b);
  EXPECT_EQ(BigInt<4>(a), q * b + r);
  EXPECT_LT(r, b);
}

TEST(BigInt, DivNoRemainder) {
  BigInt<2> a({20, 15});
  BigInt<2> b({5, 0});

  EXPECT_EQ(a.Div(b), std::make_pair(BigInt<2>({4, 3}), BigInt<2>({0, 0})));
}

TEST(BigInt, NumLeadingZeros) {
  constexpr BigInt<5> kOne = BigInt<5>::One();

  static_assert(
      BigInt<5>::kDigits - 1 == kOne.NumLeadingZeros(),
      "All digit except the last one should be zero");
  static_assert((-kOne).NumLeadingZeros() == 0, "Should have 0 leading zero's");

  static_assert(BigInt<5>::kDigits == BigInt<5>::Zero().NumLeadingZeros(), "All the digits are 0");

  EXPECT_EQ(BigInt<5>({17, 0, 0, 0, 0}).NumLeadingZeros(), BigInt<5>::kDigits - 5U);
  EXPECT_EQ(BigInt<5>({0, 4, 0, 0, 0}).NumLeadingZeros(), BigInt<5>::kDigits - 67U);
  EXPECT_EQ(BigInt<5>({0, 1, 0, 0, 17}).NumLeadingZeros(), 59U);
  EXPECT_EQ(BigInt<5>({0, 1, 0, 0, 1}).NumLeadingZeros(), 63U);
  static_assert(BigInt<5>(7).NumLeadingZeros() == BigInt<5>::kDigits - 3U, "Should work.");
}

TEST(BigInt, Multiplication) {
  constexpr size_t kN1 = 1;
  constexpr size_t kN2 = 2;
  constexpr size_t kN3 = 10;

  static_assert(
      BigInt<kN1>::Zero() * BigInt<kN1>::Zero() == BigInt<2 * kN1>::Zero(), "0*0 is not 0");
  static_assert(
      BigInt<kN3>::Zero() * BigInt<kN3>::Zero() == BigInt<2 * kN3>::Zero(), "0*0 is not 0");
  static_assert(BigInt<kN2>::One() * BigInt<kN2>::One() == BigInt<2 * kN2>::One(), "1*1 is not 1");
  static_assert(
      BigInt<kN1>(Pow2(23)) * BigInt<kN1>(Pow2(27)) == BigInt<2 * kN1>(Pow2(50)),
      "(2^23)*(2^27) is not 2^50");
  static_assert(BigInt<kN2>({0, 17}) * BigInt<kN2>({0, 15}) == BigInt<2 * kN2>({0, 0, 255, 0}));
  static_assert(
      0x45467f1b1b72b92a_Z * 0x5a24f03a01d5b10c_Z == 0x1864c79b3117812a6d564ff0d558b7f8_Z);
  static_assert(
      0x5342b50c88dbce0db6fe1c672256eb8d_Z * 0xf42ff50167e9c6cca4d5b18636b1516e_Z ==
          0x4f6b2d7e7c1233fdc642edeefc766bc635729fa19af730c8cf66b2c4dc5dd396_Z,
      "Multiplication is wrong.");
  static_assert(
      0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff_Z *
              0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff_Z ==
          BigInt<8>({0x1, 0x0, 0x0, 0x0, 0xfffffffffffffffe, 0xffffffffffffffff, 0xffffffffffffffff,
                     0xffffffffffffffff}),
      "Multiplication is wrong.");
}

TEST(BigInt, MulMod) {
  const BigInt<4> minus_one = -BigInt<4>::One();
  const BigInt<4> m = BigInt<4>(8);
  const BigInt<4> res = BigInt<4>::MulMod(minus_one, minus_one, m);
  EXPECT_EQ(res, BigInt<4>(1));

  EXPECT_EQ(BigInt<4>::MulMod(BigInt<4>(7), BigInt<4>(5), BigInt<4>(32)), BigInt<4>(3));
}

TEST(BigInt, InvMod) {
  Prng prng;
  const auto val = BigInt<4>::RandomBigInt(&prng);
  const auto prime = 0xf04a65fa008b9e14bfe07094f9ff9bb7363ae6512e213a0a104adb17fb81b385_Z;
  const auto inv = val.InvModPrime(prime);
  EXPECT_EQ(BigInt<4>::MulMod(val, inv, prime), 0x1_Z);
}

TEST(BigInt, InvMod_Zero) {
  const auto prime = 0xf04a65fa008b9e14bfe07094f9ff9bb7363ae6512e213a0a104adb17fb81b385_Z;
  EXPECT_ASSERT(BigInt<4>::Zero().InvModPrime(prime), HasSubstr("Inverse of 0"));
}

TEST(BigInt, UserLiteral) {
  auto a = 0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001_Z;
  BigInt<4> b({0xffffffff00000001, 0x53bda402fffe5bfe, 0x3339d80809a1d805, 0x73eda753299d7d48});

  static_assert(std::is_same<decltype(a), decltype(b)>::value, "decltype(a) should be BigInt<4>");

  ASSERT_EQ(a, b);
}

constexpr BigInt<1> BigWithVal(uint64_t val) {
  BigInt<1> res{0};
  res[0] = val;
  return res;
}

TEST(BigInt, ConstexprTest) {
  constexpr auto kVal = 0x18_Z;

  static_assert(kVal == 0x18_Z, "Two identical numbers should be equal");
  static_assert(kVal != 0x27_Z, "Two different numbers shouldn't be equal");

  static_assert(kVal[0] == 0x18, "Two identical numbers should be equal");
  static_assert(std::is_same<decltype(kVal[0]), const uint64_t&>::value, "Bad type");

  static_assert(BigWithVal(13)[0] == 13, "should be equal");

  static_assert(Umul128(13, 4) == __uint128_t(52), "should be equal");
  static_assert(BigInt<2>(46) < BigInt<2>(87), "should work");
  static_assert(BigInt<2>(146) >= BigInt<2>(87), "should work");
}

TEST(BigInt, BigIntWidening) {
  ASSERT_EQ(BigInt<2>({0xffffffff00000001, 0}), 0xffffffff00000001_Z);
  ASSERT_EQ(BigInt<3>({0xffffffff00000001, 0x17, 0}), BigInt<2>({0xffffffff00000001, 0x17}));
}

TEST(BigInt, ConstexprBigIntWidening) {
  static_assert(BigInt<2>(0x1_Z) != BigInt<2>(0x2_Z), "should not be equal");
}

TEST(BigInt, UserLiteral2) {
  auto zero = 0x0_Z;
  BigInt<1> one_limb_zero(0);
  BigInt<2> two_limb_zero(0);

  static_assert(
      std::is_same<decltype(zero), decltype(one_limb_zero)>::value,
      "decltype(zero) should be BigInt<1>");
  static_assert(
      !std::is_same<decltype(zero), decltype(two_limb_zero)>::value,
      "decltype(zero) shouldn't be BigInt<2>");

  ASSERT_EQ(one_limb_zero, zero);
  ASSERT_EQ(two_limb_zero,
            zero);  // Works due to implicit cast of BigInt<2> to BigInt<1>.
}

}  // namespace
}  // namespace starkware
