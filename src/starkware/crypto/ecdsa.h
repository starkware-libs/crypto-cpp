#ifndef STARKWARE_CRYPTO_ECDSA_H_
#define STARKWARE_CRYPTO_ECDSA_H_

#include "starkware/algebra/elliptic_curve.h"
#include "starkware/algebra/prime_field_element.h"

namespace starkware {

/*
  Verifies ECDSA signature of a given hash message z with a given public key.
  Returns true if either public_key or -public_key signs the message.
  NOTE: This function assumes that the public_key is on the curve.
*/
bool VerifyEcdsa(
    const EcPoint<PrimeFieldElement>& public_key, const PrimeFieldElement& z,
    const PrimeFieldElement& r, const PrimeFieldElement& w);

/*
  Same as VerifyEcdsa() except that only the x coordinate of the public key is given. The y
  coordinate is computed, and both options are checked.
*/
bool VerifyEcdsaPartialKey(
    const PrimeFieldElement& public_key_x, const PrimeFieldElement& z, const PrimeFieldElement& r,
    const PrimeFieldElement& w);

}  // namespace starkware

#endif  // STARKWARE_CRYPTO_ECDSA_H_
