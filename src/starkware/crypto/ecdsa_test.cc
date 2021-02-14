#include "starkware/crypto/ecdsa.h"

#include "gtest/gtest.h"

#include "starkware/algebra/elliptic_curve.h"
#include "starkware/algebra/prime_field_element.h"
#include "starkware/crypto/elliptic_curve_constants.h"

namespace starkware {
namespace {

TEST(ECDSA, PublicKeyFromPrivate) {
  const auto private_key = 0x3c1e9550e66958296d11b60f8e8e7a7ad990d07fa65d5f7652c4a6c87d4e3cc_Z;
  const EcPoint<PrimeFieldElement> public_key(
      PrimeFieldElement::FromBigInt(
          0x77a3b314db07c45076d11f62b6f9e748a39790441823307743cf00d6597ea43_Z),
      PrimeFieldElement::FromBigInt(
          0x54d7beec5ec728223671c627557efc5c9a6508425dc6c900b7741bf60afec06_Z));

  EXPECT_EQ(public_key, GetPublicKey(private_key));
}

TEST(ECDSA, SignAndVerify) {
  Prng prng;
  using ValueType = PrimeFieldElement::ValueType;

  // Draw test parameters.
  const auto private_key = ValueType::RandomBigInt(&prng);
  const auto public_key = GetPublicKey(private_key);
  const auto msg = PrimeFieldElement::RandomElement(&prng);
  // Choose an arbitrary value for k,
  const auto k = 0x54d7beec5ec728223671c627557efc5c9a6508425dc6c900b7741bf60afec06_Z;

  const auto signature = SignEcdsa(private_key, msg, k);
  EXPECT_TRUE(VerifyEcdsa(public_key, msg, signature));
}

TEST(VerifyEcdsa, Regression) {
  Prng prng;
  const auto alpha = GetEcConstants().k_alpha;
  const auto beta = GetEcConstants().k_beta;

  // The following hard-coded values were cross-checked with a number of implementations of the
  // ECDSA verification algorithm.
  const auto public_key_x = PrimeFieldElement::FromBigInt(
      0x77a3b314db07c45076d11f62b6f9e748a39790441823307743cf00d6597ea43_Z);
  const auto public_key_y = PrimeFieldElement::FromBigInt(
      0x54d7beec5ec728223671c627557efc5c9a6508425dc6c900b7741bf60afec06_Z);
  const EcPoint<PrimeFieldElement> public_key(public_key_x, public_key_y);
  const EcPoint<PrimeFieldElement> wrong_public_key =
      EcPoint<PrimeFieldElement>::Random(alpha, beta, &prng);

  const auto z = PrimeFieldElement::FromBigInt(
      0x397e76d1667c4454bfb83514e120583af836f8e32a516765497823eabe16a3f_Z);
  const auto r = PrimeFieldElement::FromBigInt(
      0x173fd03d8b008ee7432977ac27d1e9d1a1f6c98b1a2f05fa84a21c84c44e882_Z);
  const auto w = PrimeFieldElement::FromBigInt(
      0x1f2c44a7798f55192f153b4c48ea5c1241fbb69e6132cc8a0da9c5b62a4286e_Z);

  EXPECT_TRUE(VerifyEcdsa(public_key, z, {r, w}));
  EXPECT_TRUE(VerifyEcdsa(-public_key, z, {r, w}));
  EXPECT_FALSE(VerifyEcdsa(wrong_public_key, z, {r, w}));
  EXPECT_FALSE(VerifyEcdsa(public_key, z + PrimeFieldElement::One(), {r, w}));
  EXPECT_FALSE(VerifyEcdsa(public_key, z, {r + PrimeFieldElement::One(), w}));
  EXPECT_FALSE(VerifyEcdsa(public_key, z, {r, w + PrimeFieldElement::One()}));

  EXPECT_TRUE(VerifyEcdsaPartialKey(public_key_x, z, {r, w}));
  EXPECT_FALSE(VerifyEcdsaPartialKey(wrong_public_key.x, z, {r, w}));
  EXPECT_FALSE(VerifyEcdsaPartialKey(public_key_x, z + PrimeFieldElement::One(), {r, w}));
  EXPECT_FALSE(VerifyEcdsaPartialKey(public_key_x, z, {r + PrimeFieldElement::One(), w}));
  EXPECT_FALSE(VerifyEcdsaPartialKey(public_key_x, z, {r, w + PrimeFieldElement::One()}));
  EXPECT_FALSE(VerifyEcdsaPartialKey(public_key_y, z, {r, w}));
}

TEST(VerifyEcdsa, Benchmark) {
  Prng prng;
  for (size_t i = 0; i < 100; i++) {
    EXPECT_FALSE(VerifyEcdsa(
        {PrimeFieldElement::RandomElement(&prng), PrimeFieldElement::RandomElement(&prng)},
        PrimeFieldElement::RandomElement(&prng),
        {PrimeFieldElement::RandomElement(&prng), PrimeFieldElement::RandomElement(&prng)}));
  }
}

TEST(VerifyEcdsaPartialKey, Benchmark) {
  Prng prng;
  const auto public_key_x = PrimeFieldElement::FromBigInt(
      0x77a3b314db07c45076d11f62b6f9e748a39790441823307743cf00d6597ea43_Z);
  for (size_t i = 0; i < 100; i++) {
    EXPECT_FALSE(VerifyEcdsaPartialKey(
        public_key_x, PrimeFieldElement::RandomElement(&prng),
        {PrimeFieldElement::RandomElement(&prng), PrimeFieldElement::RandomElement(&prng)}));
  }
}

}  // namespace
}  // namespace starkware
