#include "starkware/algebra/prime_field_element.h"

#include "gtest/gtest.h"

namespace starkware {
namespace {

TEST(PrimeFieldElementTest, One) {
  auto a = PrimeFieldElement::One();
  EXPECT_EQ(a * a, a);
}

TEST(PrimeFieldElementTest, OneNegTest) {
  auto a = PrimeFieldElement::One();
  EXPECT_NE(a * a, a + a);
}

TEST(PrimeFieldElementTest, AddOneToMinusOne) {
  auto a = PrimeFieldElement::One();
  auto z = PrimeFieldElement::Zero();
  auto b = z - a;
  EXPECT_EQ(a + b, z);
}

TEST(PrimeFieldElementTest, AddSimple) {
  static_assert(PrimeFieldElement::ValueType::LimbCount() >= 4);
  const auto a = PrimeFieldElement::FromBigInt(
      0x01f5883e65f820d099915c908686b9d1c903896a679f32d65369cbe3b0000005_Z);
  const auto b = PrimeFieldElement::FromBigInt(
      0x010644e72e131a0f9b8504b68181585d94816a916871ca8d3c208c16d87cfd42_Z);
  const auto z = PrimeFieldElement::FromBigInt(
      0x02fbcd25940b3ae0351661470808122f5d84f3fbd010fd638f8a57fa887cfd47_Z);
  EXPECT_EQ(a + b, z);
}

TEST(PrimeField, Random) {
  Prng prng;
  for (size_t i = 0; i < 100; ++i) {
    auto a = PrimeFieldElement::RandomElement(&prng);
    auto b = PrimeFieldElement::RandomElement(&prng);
    EXPECT_NE(a, b);
  }
}

TEST(PrimeFieldElementTest, Inv) {
  auto a = PrimeFieldElement::One();
  auto z = PrimeFieldElement::Zero();
  auto b = z - a;

  EXPECT_EQ(b.Inverse(), b);
}

TEST(PrimeFieldElementTest, RandomInv) {
  Prng prng;
  const auto orig = PrimeFieldElement::RandomElement(&prng);
  EXPECT_EQ(orig * orig.Inverse(), PrimeFieldElement::One());
}

TEST(PrimeFieldElementTest, InvBenchmark) {
  Prng prng;
  const auto orig = PrimeFieldElement::RandomElement(&prng);
  auto val = orig;

  for (size_t i = 0; i < 10001; i++) {
    val = val.Inverse();
  }
  EXPECT_EQ(val, orig.Inverse());
}

PrimeFieldElement NaivePow(const PrimeFieldElement& base, uint64_t exponent) {
  auto res = PrimeFieldElement::One();
  for (size_t i = 0; i < exponent; i++) {
    res = res * base;
  }
  return res;
}

TEST(PrimeFieldElementTest, Pow) {
  Prng prng;
  const auto base = PrimeFieldElement::RandomElement(&prng);

  EXPECT_EQ(base.Pow(0), PrimeFieldElement::One());
  EXPECT_EQ(base.Pow({}), PrimeFieldElement::One());

  EXPECT_EQ(base.Pow(1), base);
  EXPECT_EQ(base.Pow(std::vector<bool>{1}), base);

  const auto expected = NaivePow(base, 100);
  EXPECT_EQ(base.Pow(100), expected);
  EXPECT_EQ(base.Pow({0, 0, 1, 0, 0, 1, 1}), expected);
}

TEST(PrimeFieldElementTest, PowRandom) {
  Prng prng;
  const auto base = PrimeFieldElement::RandomElement(&prng);
  const uint64_t exponent = prng.RandomUint64(0, 100);
  EXPECT_EQ(base.Pow(exponent), NaivePow(base, exponent));
}

}  // namespace
}  // namespace starkware
