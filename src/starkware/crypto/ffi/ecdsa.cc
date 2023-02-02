#include "starkware/crypto/ffi/ecdsa.h"
#include "starkware/crypto/ecdsa.h"

#include <array>

#include "third_party/gsl/gsl-lite.hpp"

#include "starkware/algebra/prime_field_element.h"
#include "starkware/crypto/ffi/utils.h"

namespace starkware {

namespace {

using ValueType = PrimeFieldElement::ValueType;

constexpr size_t kElementSize = sizeof(ValueType);
constexpr size_t kOutBufferSize = 1024;
static_assert(kOutBufferSize >= kElementSize, "kOutBufferSize is not big enough");

}  // namespace

extern "C" int GetPublicKey(
    const gsl::byte private_key[kElementSize], gsl::byte out[kElementSize]) {
  try {
    const auto stark_key = GetPublicKey(Deserialize(gsl::make_span(private_key, kElementSize))).x;
    Serialize(stark_key.ToStandardForm(), gsl::make_span(out, kElementSize));
  } catch (const std::exception& e) {
    return HandleError(e.what(), gsl::make_span(out, kOutBufferSize));
  } catch (...) {
    return HandleError("Unknown c++ exception.", gsl::make_span(out, kOutBufferSize));
  }
  return 0;
}

extern "C" int Verify(
    const gsl::byte stark_key[kElementSize], const gsl::byte msg_hash[kElementSize],
    const gsl::byte r_bytes[kElementSize], const gsl::byte w_bytes[kElementSize]) {
  try {
    // The following call will throw in case of verification failure.
    return VerifyEcdsaPartialKey(
        PrimeFieldElement::FromBigInt(Deserialize(gsl::make_span(stark_key, kElementSize))),
        PrimeFieldElement::FromBigInt(Deserialize(gsl::make_span(msg_hash, kElementSize))),
        {PrimeFieldElement::FromBigInt(Deserialize(gsl::make_span(r_bytes, kElementSize))),
         PrimeFieldElement::FromBigInt(Deserialize(gsl::make_span(w_bytes, kElementSize)))});
  } catch (...) {
    return false;
  }
  return true;
}

extern "C" int Sign(
    const gsl::byte private_key[kElementSize], const gsl::byte message[kElementSize],
    const gsl::byte k[kElementSize], gsl::byte out[kOutBufferSize]) {
  try {
    const auto sig = SignEcdsa(
        Deserialize(gsl::make_span(private_key, kElementSize)),
        PrimeFieldElement::FromBigInt(Deserialize(gsl::make_span(message, kElementSize))),
        Deserialize(gsl::make_span(k, kElementSize)));

    Serialize(sig.first.ToStandardForm(), gsl::make_span(out, kElementSize));
    Serialize(sig.second.ToStandardForm(), gsl::make_span(out + kElementSize, kElementSize));
  } catch (const std::exception& e) {
    return HandleError(e.what(), gsl::make_span(out, kOutBufferSize));
  } catch (...) {
    return HandleError("Unknown c++ exception.", gsl::make_span(out, kOutBufferSize));
  }
  return 0;
}

}  // namespace starkware
