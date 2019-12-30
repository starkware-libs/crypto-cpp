#include "starkware/algebra/elliptic_curve.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "starkware/algebra/big_int.h"
#include "starkware/algebra/fraction_field_element.h"
#include "starkware/algebra/prime_field_element.h"
#include "starkware/utils/prng.h"
#include "starkware/utils/test_utils.h"

namespace starkware {
namespace {

using testing::HasSubstr;

TEST(EllipticCurve, DoublePointTest) {
  Prng prng;
  PrimeFieldElement x = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement y = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  // There is always a beta such that {x,y} is a point on the curve y^2 = x^3 + alpha * x + beta.
  EcPoint<PrimeFieldElement> point = {x, y};
  EcPoint<PrimeFieldElement> point_times_4 = point.Double(alpha).Double(alpha);
  ASSERT_EQ(point_times_4, (point + point.Double(alpha)) + point);
}

TEST(EllipticCurve, MulByScalarTest) {
  Prng prng;
  const EcPoint<PrimeFieldElement> point = {PrimeFieldElement::RandomElement(&prng),
                                            PrimeFieldElement::RandomElement(&prng)};
  const PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  // There is always a beta such that {x,y} is a point on the curve y^2 = x^3 + alpha * x + beta.

  const EcPoint<PrimeFieldElement> point_times_5_b = point.MultiplyByScalar(0x5_Z, alpha);
  ASSERT_EQ(point_times_5_b, point.Double(alpha).Double(alpha) + point);
}

TEST(EllipticCurve, MulByZeroTest) {
  Prng prng;
  const EcPoint<PrimeFieldElement> point = {PrimeFieldElement::RandomElement(&prng),
                                            PrimeFieldElement::RandomElement(&prng)};
  const PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  // There is always a beta such that {x,y} is a point on the curve y^2 = x^3 + alpha * x + beta.

  // Multiply by zero (result should be zero).
  EXPECT_ASSERT(point.MultiplyByScalar(0x0_Z, alpha), HasSubstr("zero element"));
}

TEST(EllipticCurve, MulPowerBecomesZeroTest) {
  Prng prng;
  // Start with a point P such that P+P = zero.
  const EcPoint<PrimeFieldElement> point = {PrimeFieldElement::RandomElement(&prng),
                                            PrimeFieldElement::FromUint(0)};
  const PrimeFieldElement alpha = PrimeFieldElement::FromUint(1);

  // Check that multiplying P by an odd scalar gives back P.
  ASSERT_EQ(point.MultiplyByScalar(0x3_Z, alpha), point);
  ASSERT_EQ(point.MultiplyByScalar(0x12431234234121_Z, alpha), point);

  // Check that multiplying P by an even scalar gives zero.
  EXPECT_ASSERT(point.MultiplyByScalar(0x4_Z, alpha), HasSubstr("zero element"));
}

TEST(EllipticCurve, PlusPointTest) {
  Prng prng;
  const PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  const PrimeFieldElement beta = PrimeFieldElement::RandomElement(&prng);
  const auto point1 = EcPoint<PrimeFieldElement>::Random(alpha, beta, &prng);
  const auto point2 = EcPoint<PrimeFieldElement>::Random(alpha, beta, &prng);
  const auto point3 = EcPoint<PrimeFieldElement>::Random(alpha, beta, &prng);
  EXPECT_EQ(point1 + point2, point2 + point1);
  EXPECT_EQ((point1 + point2) + point3, point1 + (point2 + point3));
}

TEST(EllipticCurve, MinusPointTest) {
  Prng prng;
  PrimeFieldElement x1 = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement y1 = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement x2 = PrimeFieldElement::RandomElement(&prng);
  PrimeFieldElement y2 = PrimeFieldElement::RandomElement(&prng);
  // There is always a beta such that {x,y} is a point on the curve y^2 = x^3 + alpha * x + beta.
  EcPoint<PrimeFieldElement> point1 = {x1, y1};
  EcPoint<PrimeFieldElement> point2 = {x2, y2};
  EcPoint<PrimeFieldElement> point1_times_2 = point1.Double(alpha);
  ASSERT_EQ(point1_times_2 - point1, point1);
  ASSERT_EQ((point1 + point2) - point1, point2);
}

TEST(EllipticCurve, RandomPointOnCurve) {
  Prng prng;
  const PrimeFieldElement alpha = PrimeFieldElement::RandomElement(&prng);
  const PrimeFieldElement beta = PrimeFieldElement::RandomElement(&prng);
  const auto point = EcPoint<PrimeFieldElement>::Random(alpha, beta, &prng);
  // Verifies that the point is on the curve.
  EXPECT_TRUE(point.y * point.y == point.x * point.x * point.x + alpha * point.x + beta);

  const auto point2 = EcPoint<PrimeFieldElement>::GetPointFromX(point.x, alpha, beta);
  ASSERT_TRUE(point2.has_value());
  EXPECT_EQ(point.x, point2->x);
  EXPECT_TRUE(point.y == point2->y || point.y == -point2->y);
}

TEST(EllipticCurve, TestConvertTo) {
  Prng prng;
  const PrimeFieldElement first_element = PrimeFieldElement::RandomElement(&prng);
  const PrimeFieldElement second_element = PrimeFieldElement::RandomElement(&prng);
  const auto random_ec_point = EcPoint<PrimeFieldElement>(first_element, second_element);
  const auto ec_point_converted =
      random_ec_point.template ConvertTo<FractionFieldElement<PrimeFieldElement>>();
  EXPECT_EQ(random_ec_point, ec_point_converted.template ConvertTo<PrimeFieldElement>());
}

}  // namespace
}  // namespace starkware
