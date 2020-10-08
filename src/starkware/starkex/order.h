#ifndef STARKWARE_STARKEX_ORDER_H_
#define STARKWARE_STARKEX_ORDER_H_

#include "starkware/algebra/prime_field_element.h"

namespace starkware {

/*
  Serializes the order message in the canonical format expected by the verifier.
  party_a sells amount_sell coins of token_sell from vault_id_sell.
  party_a buys amount_buy coins of token_buy into vault_id_buy.
  expiration_timestamp - the number of hours that have elapsed since 01/01/1970 (i.e.,
  Unix timstamp / 3600).
*/
PrimeFieldElement GetSettlementOrderMessage(
    uint64_t vault_id_sell, uint64_t vault_id_buy, uint64_t amount_sell, uint64_t amount_buy,
    uint64_t nonce, uint64_t expiration_timestamp, const PrimeFieldElement& token_sell,
    const PrimeFieldElement& token_buy);

/*
  Serializes the transfer message in the canonical format expected by the verifier.
  The sender transfers amount coins of type token to target_public_key, from sender_vault_id
  to target_vault_id.
  expiration_timestamp - the number of hours that have elapsed since 01/01/1970 (i.e.,
  Unix timstamp / 3600).
*/
PrimeFieldElement GetTransferOrderMessage(
    uint64_t sender_vault_id, uint64_t target_vault_id, uint64_t amount, uint64_t nonce,
    uint64_t expiration_timestamp, const PrimeFieldElement& token,
    const PrimeFieldElement& target_public_key);

/*
  Serializes the conditional transfer message in the canonical format expected by the verifier.
  The sender transfers amount coins of type token to target_public_key, from sender_vault_id
  to target_vault_id, only if condition is valid.
  expiration_timestamp - the number of hours that have elapsed since 01/01/1970 (i.e.,
  Unix timstamp / 3600).
*/
PrimeFieldElement GetConditionalTransferOrderMessage(
    uint64_t sender_vault_id, uint64_t target_vault_id, uint64_t amount, uint64_t nonce,
    uint64_t expiration_timestamp, const PrimeFieldElement& token,
    const PrimeFieldElement& target_public_key, const PrimeFieldElement& condition);

/*
  Returns the 63-bit order id based on the order message (the result of GetSettlementOrderMessage()
  or GetTransferOrderMessage()).
*/
uint64_t GetOrderIdFromMessage(const PrimeFieldElement& message);

}  // namespace starkware

#endif  // STARKWARE_STARKEX_ORDER_H_
