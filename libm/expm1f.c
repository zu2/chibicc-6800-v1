//
// expm1f(x) = e^x - 1
//
// Cody-Waite range reduction
//
// e^x - 1 = 2^n * (1 + t) - 1,  t = e^r - 1,  r = x - n * ln2
//
// 2^n * (1 + t) - 1 = 2^n * ((1 - 2^-n) + t)
//
#include <math.h>

// Cody-Waite: LN2_HI holds the top 24 bits of ln2, LN2_LO the rest
#define LN2_HI   0x1.62e000p-1f
#define LN2_LO   0x1.0bfbe8p-15f
#define INV_LN2  0x1.715476p+0f

// exp(r) - 1 = r + r*r*P(r) on |r| <= ln2/2, minimax fit
#define P0  0x1.000000p-1f
#define P1  0x1.5554dcp-3f
#define P2  0x1.55551ap-5f
#define P3  0x1.120b6cp-7f
#define P4  0x1.6d1106p-10f

float expm1f(float x)
{
  if (x == 0.0f)
    return x;
  if (isnan(x))
    return x;
  if (x > 0x1.62e430p+6f)     // ln(FLT_MAX)
    return INFINITY;
  if (x < -0x1.154246p+4f)    // e^x - 1 rounds to -1.0f
    return -1.0f;

  if (fabsf(x) <= 0x1.62e430p-2f) { // ln2 / 2
    float x2 = x * x;
    float p = P0 + x * (P1 + x * (P2 + x * (P3 + x * P4)));
    return x + x2 * p;
  }

  float n_f = roundf(x * INV_LN2);
  int n = (int)n_f;

  float r = (x - n_f * LN2_HI) - n_f * LN2_LO;

  float r2 = r * r;
  float p = P0 + r * (P1 + r * (P2 + r * (P3 + r * P4)));
  float t = r + r2 * p;

  if (n >= 25)
    return ldexpf(1.0f + t, n);

  // 1 - 2^-n is exact for n <= 24, so t stays inside the last rounding
  if (n >= 1)
    return ldexpf((1.0f - ldexpf(1.0f, -n)) + t, n);
  return ldexpf(1.0f + t, n) - 1.0f;
}
