#include <math.h>
#include <float.h>

#define C0 1.00000000f
#define C1 0.99999994f
#define C2 0.50000012f
#define C3 0.16666599f
#define C4 0.04166901f
#define C5 0.00833123f
#define C6 0.00139145f
#define C7 0.00019712f


float expf(float x)
{
  // Handle special cases
  if (isnan(x)) {
    return x;
  }
  if (x == 0.0f) {
    return 1.0f;
  }
  if (x == INFINITY) {
    return INFINITY;
  }
  if (x == -INFINITY) {
    return 0.0f;
  }
  if (x > 0x1.62e43000p+6) {    // 88.72284f;
    return INFINITY;
  }
  if (x == 88.722839f) {
    return FLT_MAX;
  }
  if (x < -103.97208f) {
    return 0.0f;
  }

  // Range reduction using frexpf for higher accuracy
  // Step 1: Calculate n such that x = n*ln2 + r, r in [-ln2/2, ln2/2]

  // Range reduction
  int n = (int)(x / M_LN2);
  float r = fmodf(x, M_LN2);
  if (r > 0.5f * M_LN2) {
    r -= M_LN2;
    n += 1;
  } else if (r < -0.5f * M_LN2) {
    r += M_LN2;
    n -= 1;
  }

  // Step 2: Polynomial approximation of exp(r) using Horner's method
  float exp_r = C0 + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));

  // Step 3: Combine using ldexpf
  return ldexpf(exp_r, n);
}
