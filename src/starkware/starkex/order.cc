#include "starkware/starkex/order.h"

#include "starkware/crypto/pedersen_hash.h"

namespace starkware {

namespace {

PrimeFieldElement GetOrderPackedMessage(
    uint64_t order_type, uint64_t vault_a, uint64_t vault_b, uint64_t amount_a, uint64_t amount_b,
    uint64_t nonce, uint64_t expiration_timestamp) {
  static constexpr uint64_t kOrderTypeLimit = 3;
  static constexpr uint64_t kVaultIdLimit = Pow2(31);
  static constexpr uint64_t kAmountLimit = Pow2(63);
  static constexpr uint64_t kNonceLimit = Pow2(31);
  static constexpr uint64_t kExpirationTimestampLimit = Pow2(22);

  ASSERT(order_type < kOrderTypeLimit, "Invalid order_type.");
  ASSERT(vault_a < kVaultIdLimit, "vault_a is out of range.");
  ASSERT(vault_b < kVaultIdLimit, "vault_b is out of range.");
  ASSERT(amount_a < kAmountLimit, "amount_a is out of range.");
  ASSERT(amount_b < kAmountLimit, "amount_b is out of range.");
  ASSERT(nonce < kNonceLimit, "nonce is out of range.");
  ASSERT(expiration_timestamp < kExpirationTimestampLimit, "expiration_timestamp is out of range.");

  PrimeFieldElement packed_message = PrimeFieldElement::FromUint(order_type);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kVaultIdLimit)) +
                   PrimeFieldElement::FromUint(vault_a);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kVaultIdLimit)) +
                   PrimeFieldElement::FromUint(vault_b);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kAmountLimit)) +
                   PrimeFieldElement::FromUint(amount_a);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kAmountLimit)) +
                   PrimeFieldElement::FromUint(amount_b);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kNonceLimit)) +
                   PrimeFieldElement::FromUint(nonce);
  packed_message = (packed_message * PrimeFieldElement::FromUint(kExpirationTimestampLimit)) +
                   PrimeFieldElement::FromUint(expiration_timestamp);
  return packed_message;
}

}  // namespace

PrimeFieldElement GetSettlementOrderMessage(
    uint64_t vault_id_sell, uint64_t vault_id_buy, uint64_t amount_sell, uint64_t amount_buy,
    uint64_t nonce, uint64_t expiration_timestamp, const PrimeFieldElement& token_sell,
    const PrimeFieldElement& token_buy) {
  return PedersenHash(
      PedersenHash(token_sell, token_buy),
      GetOrderPackedMessage(
          0, vault_id_sell, vault_id_buy, amount_sell, amount_buy, nonce, expiration_timestamp));
}

PrimeFieldElement GetTransferOrderMessage(
    uint64_t sender_vault_id, uint64_t target_vault_id, uint64_t amount, uint64_t nonce,
    uint64_t expiration_timestamp, const PrimeFieldElement& token,
    const PrimeFieldElement& target_public_key) {
  return PedersenHash(
      PedersenHash(token, target_public_key),
      GetOrderPackedMessage(
          1, sender_vault_id, target_vault_id, amount, 0, nonce, expiration_timestamp));
}

PrimeFieldElement GetConditionalTransferOrderMessage(
    uint64_t sender_vault_id, uint64_t target_vault_id, uint64_t amount, uint64_t nonce,
    uint64_t expiration_timestamp, const PrimeFieldElement& token,
    const PrimeFieldElement& target_public_key, const PrimeFieldElement& condition) {
  return PedersenHash(
      PedersenHash(PedersenHash(token, target_public_key), condition),
      GetOrderPackedMessage(
          2, sender_vault_id, target_vault_id, amount, 0, nonce, expiration_timestamp));
}

uint64_t GetOrderIdFromMessage(const PrimeFieldElement& message) {
  const auto bigint = message.ToStandardForm();
  static_assert(bigint.LimbCount() == 4);
  // bigint must be smaller than 2^251, so the high limb must be below 2^59 (59 = 251 - 64 * 3).
  ASSERT(bigint[3] < Pow2(59), "message is out of range.");
  // Order id is the 63 high bits of a 251-bit number, so we take 60 bits from the high limb and 4
  // more bits from the next limb.
  return (bigint[3] << 4) | (bigint[2] >> 60);
}

}  // namespace starkware
