#include "starkware/crypto/ecdsa.h"

#include "starkware/algebra/fraction_field_element.h"
#include "starkware/crypto/elliptic_curve_constants.h"
#include "starkware/utils/error_handling.h"

namespace starkware {

bool VerifyEcdsa(
    const EcPoint<PrimeFieldElement>& public_key, const PrimeFieldElement& z,
    const PrimeFieldElement& r, const PrimeFieldElement& w) {
  using FractionFieldElementT = FractionFieldElement<PrimeFieldElement>;
  using EcPointT = EcPoint<FractionFieldElementT>;
  // z, r, w should be smaller than 2^251.
  const auto upper_bound = 0x800000000000000000000000000000000000000000000000000000000000000_Z;
  ASSERT(z != PrimeFieldElement::Zero(), "Message cannot be zero.");
  ASSERT(z.ToStandardForm() < upper_bound, "z is too big.");
  ASSERT(r != PrimeFieldElement::Zero(), "r cannot be zero.");
  ASSERT(r.ToStandardForm() < upper_bound, "r is too big.");
  ASSERT(w != PrimeFieldElement::Zero(), "w cannot be zero.");
  ASSERT(w.ToStandardForm() < upper_bound, "w is too big.");
  const FractionFieldElementT alpha(GetEcConstants().k_alpha);
  const auto generator = GetEcConstants().k_points[1];
  const auto zw = PrimeFieldElement::ValueType::MulMod(
      z.ToStandardForm(), w.ToStandardForm(), GetEcConstants().k_order);
  const EcPointT zw_g = generator.ConvertTo<FractionFieldElementT>().MultiplyByScalar(zw, alpha);
  const auto rw = PrimeFieldElement::ValueType::MulMod(
      r.ToStandardForm(), w.ToStandardForm(), GetEcConstants().k_order);
  const EcPointT rw_q = public_key.ConvertTo<FractionFieldElementT>().MultiplyByScalar(rw, alpha);
  return (zw_g + rw_q).x.ToBaseFieldElement() == r || (zw_g - rw_q).x.ToBaseFieldElement() == r;
}

bool VerifyEcdsaPartialKey(
    const PrimeFieldElement& public_key_x, const PrimeFieldElement& z, const PrimeFieldElement& r,
    const PrimeFieldElement& w) {
  const auto alpha = GetEcConstants().k_alpha;
  const auto beta = GetEcConstants().k_beta;
  const auto public_key = EcPoint<PrimeFieldElement>::GetPointFromX(public_key_x, alpha, beta);
  ASSERT(
      public_key.has_value(), "Given public key (" + public_key_x.ToString() +
                                  ") does not correspond to a valid point on the elliptic curve.");

  // There are two points on the elliptic curve with the given public_key_x, both will be
  // tested by VerifyEcdsa().
  return VerifyEcdsa(*public_key, z, r, w);
}

}  // namespace starkware
