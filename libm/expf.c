#include <math.h>
#include <float.h>

// 7th order Taylor expansion coefficients (precomputed)
#define C0 1.0f
#define C1 1.0f
#define C2 0.5f
#define C3 0.16666667f // 1/6
#define C4 0.04166667f // 1/24
#define C5 0.00833333f // 1/120
#define C6 0.00138889f // 1/720
#define C7 0.00019841f // 1/5040

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
  if (x > 88.722839f) {
    return INFINITY;
  }
  if (x < -103.97208f) {
    return 0.0f;
  }

  // Range reduction using frexpf for higher accuracy
  // Step 1: Calculate n such that x = n*ln2 + r, r in [-ln2/2, ln2/2]
#if 0
  float n_float = x * M_LOG2E;
  int n;
  float frac = frexpf(n_float, &n); // n_float = frac * 2^n, frac in [0.5, 1)
  n = (int)(n_float + (x >= 0 ? 0.5f : -0.5f)); // round to nearest int
  float r = x - n * M_LN2;
#else
  // Range reduction using fmodf
  int n = (int)(x / M_LN2);
  float r = fmodf(x, M_LN2);
  if (r > 0.5f * M_LN2) {
    r -= M_LN2;
    n += 1;
  } else if (r < -0.5f * M_LN2) {
    r += M_LN2;
    n -= 1;
  }
#endif

  // Step 2: Polynomial approximation of exp(r) using Horner's method
  float exp_r = C0 + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * (C7)))))));

  // Step 3: Combine using ldexpf
  return ldexpf(exp_r, n);
}
