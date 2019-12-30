#include <cstddef>

#include "gtest/gtest.h"

#include "starkware/algebra/prime_field_element.h"
#include "starkware/crypto/elliptic_curve_constants.h"
#include "starkware/utils/prng.h"
#include "starkware/utils/test_utils.h"

namespace starkware {
namespace {

TEST(EllipticCurveConstants, MulByCurveOrderTest) {
  Prng prng;
  using FieldValueType = EllipticCurveConstants<PrimeFieldElement>::ValueType;
  const EcPoint<PrimeFieldElement> generator = GetEcConstants().k_points[1];
  const FieldValueType random_power = FieldValueType(prng.RandomUint64(1, 1000));
  // Multiply by the group order (result should be zero).
  const auto alpha = GetEcConstants().k_alpha;
  const auto order = GetEcConstants().k_order;
  EXPECT_ASSERT(generator.MultiplyByScalar(order, alpha), testing::HasSubstr("zero element"));
  EXPECT_EQ(
      generator.MultiplyByScalar(random_power, alpha),
      generator.MultiplyByScalar(order + random_power, alpha));
}

}  // namespace
}  // namespace starkware
