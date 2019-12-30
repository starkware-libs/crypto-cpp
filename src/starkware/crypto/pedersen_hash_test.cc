#include "starkware/crypto/pedersen_hash.h"

#include "gtest/gtest.h"

namespace starkware {
namespace {

TEST(PedersenHash, Zero) {
  EXPECT_EQ(
      PedersenHash(PrimeFieldElement::Zero(), PrimeFieldElement::Zero()),
      PrimeFieldElement::FromBigInt(
          0x49ee3eba8c1600700ee1b87eb599f16716b0b1022947733551fde4050ca6804_Z));
}

TEST(PedersenHash, Correctness) {
  const auto x = PrimeFieldElement::FromBigInt(
      0x3d937c035c878245caf64531a5756109c53068da139362728feb561405371cb_Z);
  const auto y = PrimeFieldElement::FromBigInt(
      0x208a0a10250e382e1e4bbe2880906c2791bf6275695e02fbbc6aeff9cd8b31a_Z);
  const auto expected_result = PrimeFieldElement::FromBigInt(
      0x30e480bed5fe53fa909cc0f8c4d99b8f9f2c016be4c41e13a4848797979c662_Z);
  EXPECT_EQ(PedersenHash(x, y), expected_result);
}

TEST(PedersenHash, Benchmark) {
  Prng prng;
  auto res = PrimeFieldElement::Zero();
  for (size_t i = 0; i < 1000; i++) {
    const auto y = PrimeFieldElement::RandomElement(&prng);
    res = PedersenHash(res, y);
  }
  EXPECT_NE(res, PrimeFieldElement::Zero());
}

}  // namespace
}  // namespace starkware
