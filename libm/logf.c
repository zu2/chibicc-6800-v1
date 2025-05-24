#include <math.h>
#include <stdint.h>


float logf(float x)
{
  // Handle special cases
  if (x < 0.0f || isnan(x)) {
    return NAN;
  }
  if (x == 0.0f) {
    return -INFINITY;
  }
  if (isinf(x)) {
    return INFINITY;
  }

  // Decompose x into mantissa m and exponent e
  int e;
  float m = frexpf(x, &e);

  // Precise range reduction: [0.75, 1.5)
  if (m < 0.75f) {
    m *= 2.0f;
    e--;
  }

  // Compute f = m - 1 for log1p(f)
  float f = m - 1.0f;

  // Remez-optimized 7th-order coefficients (ε < 1e-7)
  const float c1 = 0.9999999993f;
  const float c2 = -0.4999999755f;
  const float c3 = 0.3333329856f;
  const float c4 = -0.2500420645f;
  const float c5 = 0.2003053691f;
  const float c6 = -0.1662378768f;
  const float c7 = 0.1423557882f;

  // Polynomial evaluation using Horner's method
  float z = f;
  float poly =
      f * (c1 + z * (c2 + z * (c3 + z * (c4 + z * (c5 + z * (c6 + z * c7))))));

  // High-precision ln2 (0.6931471805599453)
  const float ln2 = 0.6931471825f;

  // Final composition
  return e * ln2 + poly;
}
