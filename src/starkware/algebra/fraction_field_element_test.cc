#include <limits>
#include <vector>

#include "starkware/algebra/fraction_field_element.h"
#include "starkware/algebra/prime_field_element.h"
#include "starkware/utils/test_utils.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace starkware {
namespace {

using FractionFieldElementT = FractionFieldElement<PrimeFieldElement>;

FractionFieldElementT ElementFromInts(uint64_t numerator, uint64_t denominator) {
  const FractionFieldElementT num(PrimeFieldElement::FromUint(numerator));
  const FractionFieldElementT denom(PrimeFieldElement::FromUint(denominator));
  return num * denom.Inverse();
}

TEST(FractionFieldElement, Equality) {
  FractionFieldElementT a = ElementFromInts(5, 10);
  FractionFieldElementT b = ElementFromInts(30, 60);
  FractionFieldElementT c = ElementFromInts(1, 3);
  FractionFieldElementT d(-PrimeFieldElement::One(), -PrimeFieldElement::FromUint(2));
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a == d);
  EXPECT_FALSE(a != d);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a == c);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(b == c);
  EXPECT_TRUE(b != c);
}

TEST(FractionFieldElement, Addition) {
  EXPECT_EQ(ElementFromInts(1, 3) + ElementFromInts(5, 4), ElementFromInts(19, 12));
}

TEST(FractionFieldElement, UnaryMinus) {
  Prng prng;
  static constexpr uint64_t kMaxVal = std::numeric_limits<uint64_t>::max();
  EXPECT_EQ(FractionFieldElementT::Zero(), -FractionFieldElementT::Zero());
  for (size_t i = 0; i < 100; ++i) {
    FractionFieldElementT x =
        ElementFromInts(prng.RandomUint64(1, kMaxVal), prng.RandomUint64(1, kMaxVal));
    ASSERT_NE(x, -x);
    ASSERT_EQ(FractionFieldElementT::Zero(), -x + x);
  }
}

TEST(FractionFieldElement, Subtraction) {
  FractionFieldElementT a = ElementFromInts(5, 2);
  FractionFieldElementT b = ElementFromInts(1, 3);
  FractionFieldElementT c = ElementFromInts(13, 6);
  EXPECT_EQ(a - b, c);
  EXPECT_EQ(b - a, -c);
}

TEST(FractionFieldElement, Multiplication) {
  FractionFieldElementT a = ElementFromInts(1, 3);
  FractionFieldElementT b = ElementFromInts(6, 4);
  FractionFieldElementT c1 = ElementFromInts(6, 12);
  FractionFieldElementT c2 = ElementFromInts(1, 2);
  FractionFieldElementT c3 = ElementFromInts(2, 4);
  FractionFieldElementT a_mul_b = a * b;
  EXPECT_EQ(a_mul_b, c1);
  EXPECT_EQ(a_mul_b, c2);
  EXPECT_EQ(a_mul_b, c3);
  EXPECT_EQ(a, a_mul_b * b.Inverse());
}

TEST(FractionFieldElement, Inverse) {
  Prng prng;
  FractionFieldElementT a = ElementFromInts(6, 4);
  FractionFieldElementT b = ElementFromInts(10, 1);
  FractionFieldElementT c = ElementFromInts(1, 1);
  FractionFieldElementT d = ElementFromInts(0, 1);
  FractionFieldElementT e = ElementFromInts(12, 18);
  FractionFieldElementT a_inv = a.Inverse();
  FractionFieldElementT b_inv = b.Inverse();
  FractionFieldElementT c_inv = c.Inverse();
  FractionFieldElementT e_inv = e.Inverse();
  FractionFieldElementT random = FractionFieldElementT::RandomElement(&prng);
  EXPECT_EQ(a_inv, e);
  EXPECT_EQ(a, e_inv);
  EXPECT_EQ(FractionFieldElementT::One(), a_inv * a);
  EXPECT_EQ(FractionFieldElementT::One(), b * b_inv);
  EXPECT_EQ(c, c_inv);
  EXPECT_EQ(random, random.Inverse().Inverse());
  EXPECT_ASSERT(d.Inverse(), testing::HasSubstr("Zero does not have an inverse"));
  EXPECT_EQ(random * random.Inverse(), FractionFieldElementT::One());
}

TEST(FractionFieldElement, Division) {
  FractionFieldElementT a = ElementFromInts(5, 10);
  FractionFieldElementT b = ElementFromInts(6, 4);
  FractionFieldElementT c = ElementFromInts(1, 3);
  FractionFieldElementT a_div_b = a / b;
  EXPECT_EQ(a_div_b, c);
  EXPECT_EQ(a, a_div_b * b);
}

TEST(FractionFieldElement, ToBaseFieldElement) {
  Prng prng;
  PrimeFieldElement a = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement b = PrimeFieldElement::RandomElement(&prng);
  EXPECT_EQ(FractionFieldElementT(a).ToBaseFieldElement(), a);
  EXPECT_EQ(
      FractionFieldElementT(PrimeFieldElement::Zero()).ToBaseFieldElement(),
      PrimeFieldElement::Zero());
  EXPECT_EQ(
      FractionFieldElementT(PrimeFieldElement::One()).ToBaseFieldElement(),
      PrimeFieldElement::One());
  EXPECT_EQ(
      (FractionFieldElementT(a) / FractionFieldElementT(b)).ToBaseFieldElement(), a * b.Inverse());
}

}  // namespace
}  // namespace starkware
