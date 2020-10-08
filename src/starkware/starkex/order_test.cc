#include "starkware/starkex/order.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "starkware/utils/test_utils.h"

namespace starkware {
namespace {

using testing::HasSubstr;

TEST(SettlementOrder, Regression) {
  // The following hard-coded values were cross-checked with a number of implementations.
  const auto message = GetSettlementOrderMessage(
      21, 27, 2154686749748910716, 1470242115489520459, 0, 438953,
      PrimeFieldElement::FromBigInt(
          0x5fa3383597691ea9d827a79e1a4f0f7989c35ced18ca9619de8ab97e661020_Z),
      PrimeFieldElement::FromBigInt(
          0x774961c824a3b0fb3d2965f01471c9c7734bf8dbde659e0c08dca2ef18d56a_Z));
  const auto expected_message = PrimeFieldElement::FromBigInt(
      0x397e76d1667c4454bfb83514e120583af836f8e32a516765497823eabe16a3f_Z);
  EXPECT_EQ(message, expected_message);
  EXPECT_EQ(GetOrderIdFromMessage(message), 4142879348967097428);
}

TEST(TransferOrder, Regression) {
  // The following hard-coded values were cross-checked with a number of implementations.
  const auto message = GetTransferOrderMessage(
      34, 21, 2154549703648910716, 1, 438953,
      PrimeFieldElement::FromBigInt(
          0x3003a65651d3b9fb2eff934a4416db301afd112a8492aaf8d7297fc87dcd9f4_Z),
      PrimeFieldElement::FromBigInt(
          0x5fa3383597691ea9d827a79e1a4f0f7949435ced18ca9619de8ab97e661020_Z));
  const auto expected_message = PrimeFieldElement::FromBigInt(
      0x6366b00c218fb4c8a8b142ca482145e8513c78e00faa0de76298ba14fc37ae7_Z);
  EXPECT_EQ(message, expected_message);
  EXPECT_EQ(GetOrderIdFromMessage(message), 7162605823528514760);
}

TEST(ConditionalTransferOrder, Regression) {
  // The following hard-coded values were cross-checked with a number of implementations.
  const auto message = GetConditionalTransferOrderMessage(
      34, 21, 2154549703648910716, 1, 438953,
      PrimeFieldElement::FromBigInt(
          0x3003a65651d3b9fb2eff934a4416db301afd112a8492aaf8d7297fc87dcd9f4_Z),
      PrimeFieldElement::FromBigInt(
          0x5fa3383597691ea9d827a79e1a4f0f7949435ced18ca9619de8ab97e661020_Z),
      PrimeFieldElement::FromBigInt(
          0x318ff6d26cf3175c77668cd6434ab34d31e59f806a6a7c06d08215bccb7eaf8_Z));
  const auto expected_message = PrimeFieldElement::FromBigInt(
      0xfa5f0ad1ebff93c9e6474379a213ba1e1f9e42f5f1cb361b0327e073720384_Z);
  EXPECT_EQ(message, expected_message);
  EXPECT_EQ(GetOrderIdFromMessage(message), 1127571908062083388);
}

TEST(GetOrderIdFromMessage, Correctness) {
  Prng prng;
  const uint64_t order_id = prng.RandomUint64(0, Pow2(63) - 1);
  const auto message =
      PrimeFieldElement::FromUint(order_id) * PrimeFieldElement::FromUint(2).Pow(251 - 63);
  EXPECT_EQ(GetOrderIdFromMessage(message), order_id);
}

TEST(GetOrderIdFromMessage, MessageTooLarge) {
  Prng prng;
  const auto message = PrimeFieldElement::FromUint(2).Pow(251);
  EXPECT_ASSERT(GetOrderIdFromMessage(message), HasSubstr("message is out of range"));
}

}  // namespace
}  // namespace starkware
